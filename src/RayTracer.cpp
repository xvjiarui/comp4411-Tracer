// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"
#include "fileio/bitmap.h"
#include "math.h"
extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), traceUI->getDepth() ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) && depth >=0) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		if (traceUI->isEnableTextureMapping())
		{
			return SphereInverse(r, i);
		}
		const Material& m = i.getMaterial();
		vec3f Intensity = m.shade(scene, r, i);
		vec3f reflection = 2 * ((-r.getDirection().dot(i.N)) * i.N) + r.getDirection();
		ray reflection_ray = ray(r.at(i.t), reflection.normalize());
		Intensity += prod(m.kr,traceRay(scene, reflection_ray, vec3f(1.0,1.0,1.0), depth - 1));

		vec3f conPoint = r.at(i.t); 
		vec3f normal;
		vec3f Rdir = 2 * (i.N*-r.getDirection()) * i.N - (-r.getDirection());
		// Refraction part
		// We maintain a map, this map has order so it can be simulated as a extended stack	
		const double fresnel_coeff = getFresnelCoeff(i, r);	  
		if (!i.getMaterial().kt.iszero())
		{
			// take account total refraction effect
			bool TotalRefraction = false; 
			// opposite ray
			ray oppR(conPoint, r.getDirection()); //without refraction
		
			// marker to simulate a stack
			bool toAdd = false, toErase = false;

			// For now, the interior is just hardcoded
			// That is, we judge it according to cap and whether it is box
			if (i.obj->hasInterior())
			{
				// refractive index
				double indexA, indexB;

				// For ray go out of an object
				if (i.N*r.getDirection() > RAY_EPSILON)
				{
					if (mediaHistory.empty())
					{
						indexA = 1.0;
					}
					else
					{
						// return the refractive index of last object
						indexA = mediaHistory.rbegin()->second.index;
					}

					mediaHistory.erase(i.obj->getOrder());
					toAdd = true;
					if (mediaHistory.empty())
					{
						indexB = 1.0;
					}
					else
					{
						indexB = mediaHistory.rbegin()->second.index;
					}
					normal = -i.N;
				}
				// For ray get in the object
				else
				{
					if (mediaHistory.empty())
					{
						indexA = 1.0;
					}
					else
					{
						indexA = mediaHistory.rbegin()->second.index;
					}
					mediaHistory.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
					toErase = true;
					indexB = mediaHistory.rbegin()->second.index;
					normal = i.N;
				}

				double indexRatio = indexA / indexB;
				double cos_i = max(min(normal*((-r.getDirection()).normalize()), 1.0), -1.0); //SYSNOTE: min(x, 1.0) to prevent cos_i becomes bigger than 1
				double sin_i = sqrt(1 - cos_i*cos_i);
				double sin_t = sin_i * indexRatio;

				if (sin_t > 1.0)
				{
					TotalRefraction = true;
				}
				else
				{
					TotalRefraction = false;
					double cos_t = sqrt(1 - sin_t*sin_t);
					vec3f Tdir = (indexRatio*cos_i - cos_t)*normal - indexRatio*-r.getDirection();
					oppR = ray(conPoint, Tdir);
					if (!traceUI->isEnableFresnel()) {
						Intensity += prod(i.getMaterial().kt, traceRay(scene, oppR, thresh, depth + 1));
					}
					else
					{
						Intensity += ((1 - fresnel_coeff)*prod(i.getMaterial().kt, traceRay(scene, oppR, thresh, depth + 1)));
					}
				}
			}

			if (toAdd)
			{
				mediaHistory.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
			}
			if (toErase)
			{
				mediaHistory.erase(i.obj->getOrder());
			}
		}
		return Intensity;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		if (backgroundImage && depth==traceUI->getDepth())
		{
			return getbackgroundColor(scene->getCamera()->getnx(),scene->getCamera()->getny());
		}
		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
	backgroundImage = NULL;
	textureMappingImage = NULL;

}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::loadbackgroundImage(char* fn){
	unsigned char* data = NULL;
	data = readBMP(fn, background_width, background_height);
	if (data)
	{
		if (backgroundImage)
		{
			delete []backgroundImage;
		}
		backgroundImage=data;
	}
}

void RayTracer::loadtextureMappingImage(char* fn){
	unsigned char* data = NULL;
	data = readBMP(fn, texture_width, texture_height);
	if (data)
	{
		if (textureMappingImage)
		{
			delete []textureMappingImage;
		}
		textureMappingImage=data;
	}
}

vec3f RayTracer::getbackgroundColor(double x, double y){
	if (!backgroundImage)
	{		
		return vec3f(0.0,0.0,0.0);
	}
	if (x<0||x>=1||y<0||y>=1)
	{
		return vec3f(0.0,0.0,0.0);
	}
	int x1= x*background_width;
	int y1= y*background_height;
	double v1 = backgroundImage[(y1*background_width+x1)*3]/255.0;
	double v2 = backgroundImage[(y1*background_width+x1)*3+1]/255.0;
	double v3 = backgroundImage[(y1*background_width+x1)*3+2]/255.0;
	return vec3f(v1,v2,v3);
}

vec3f RayTracer::gettextureColor(double x, double y){
	if (!textureMappingImage)
	{		
		return vec3f(0.0,0.0,0.0);
	}
	if (x<0||x>=1||y<0||y>=1)
	{
		return vec3f(0.0,0.0,0.0);
	}
	int x1= x*texture_width;
	int y1= y*texture_height;
	double v1 = textureMappingImage[(y1*texture_width+x1)*3]/255.0;
	double v2 = textureMappingImage[(y1*texture_width+x1)*3+1]/255.0;
	double v3 = textureMappingImage[(y1*texture_width+x1)*3+2]/255.0;
	return vec3f(v1,v2,v3);
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	// double x = double(i)/double(buffer_width);
	// double y = double(j)/double(buffer_height);
	// col = trace( scene,x,y );
	
	if (traceUI->isEnableJittering())
	{
		double jitter_x = double(rand() % 10 - 5)/10.0;
		double jitter_y = double(rand() % 10 - 5)/10.0;
		double x = double(i + jitter_x)/double(buffer_width);
		double y = double(j + jitter_y)/double(buffer_height);
		col = trace( scene, x, y);
	}
	else
	{
		int range = traceUI->getAntialiasingSize();
		if (range == 0)
		{
			double x = double(i)/double(buffer_width);
			double y = double(j)/double(buffer_height);
			col = trace( scene, x, y);
		}
		else 
		{
			for (int m = 0; m < range + 1; ++m)
			{
				double step = 1.0/range; 
				for (int n = 0; n < range + 1; ++n)
				{
					double x = double(i - 0.5 + m * step)/double(buffer_width);
					double y = double(j - 0.5 + n * step)/double(buffer_height);
					col += trace( scene, x, y);
				}
			}
			col = col / (range + 1) / (range + 1);
		}
	}
	


	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

double RayTracer::getFresnelCoeff(isect& i, const ray& r)
{
	if (!traceUI->isEnableFresnel())
	{
		return 1.0;
	}
	vec3f normal;
	if (i.obj->hasInterior())
	{
		double indexA, indexB;
		if (i.N*r.getDirection() > RAY_EPSILON)
		{
			if (mediaHistory.empty())
			{
				indexA = 1.0;
			}
			else
			{
				indexA = mediaHistory.rbegin()->second.index;
			}
			mediaHistory.erase(i.obj->getOrder());
			if (mediaHistory.empty())
			{
				indexB = 1.0;
			}
			else
			{
				indexB = mediaHistory.rbegin()->second.index;
			}
			normal = -i.N;
			mediaHistory.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
		}
		// For ray get in the object
		else
		{
			if (mediaHistory.empty())
			{
				indexA = 1.0;
			}
			else
			{
				indexA = mediaHistory.rbegin()->second.index;
			}
			normal = i.N;
			mediaHistory.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
			indexB = mediaHistory.rbegin()->second.index;
			mediaHistory.erase(i.obj->getOrder());
		}

		double r0 = (indexA - indexB) / (indexA + indexB);
		r0 = r0 * r0;

		const double cos_i = max(min(i.N.dot(-r.getDirection().normalize()), 1.0),-1.0);
		double sin_i = sqrt(1 - cos_i*cos_i);
		double sin_t = sin_i * (indexA/ indexB);

		if (indexA <= indexB)
		{
			return r0 + (1 - r0)*pow(1 - cos_i, 5);
		}
		else
		{
			if (sin_t > 1.0)
			{
				return 1.0;
			}
			else
			{
				double cos_t = sqrt(1 - sin_t*sin_t);
				return r0 + (1 - r0) * pow(1 - cos_t, 5);
			}
		}
	}
	else
	{
		return 1.0;
	}
}
vec3f RayTracer::SphereInverse(const ray& r, isect& i)
{
	vec3f Sp = vec3f(0, 1, 0);
	vec3f Se = vec3f(1, 0, 0);
	vec3f Sn = i.N.normalize();
	double pipipi = 3.1415926535;
	double phi = acos(-Sn.dot(Sp));
	double v = phi/pipipi;
	double theta = acos((Se.dot(Sn))/sin(phi))/2/pipipi;
	double u = theta;
	if (Sp.cross(Se).dot(Sn) <= 0)
	{
		u = 1- theta;
	}
	return gettextureColor(u, v);

}