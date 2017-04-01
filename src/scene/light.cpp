#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"
#include "math.h"
extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    vec3f d = getDirection(P);

	// loop to get the attenuation
	vec3f curP = P;
	isect isecP;
	vec3f ret = getColor(P);
	ray r = ray(curP, d);
	while (scene->intersect(r, isecP))
	{
		//if not transparent return black
		if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0);
		//use current intersection point as new light source
		curP = r.at(isecP.t);
		r = ray(curP, d);
		ret = prod(ret, isecP.getMaterial().kt);
	}
	return ret;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double a = traceUI->getAttenuationConstant();
	double b = traceUI->getAttenuationLinear();
	double c = traceUI->getAttenuationQuadratic();
	double dist = (P - position).length();
	double drop = 1.0/(a + b * dist +c * dist * dist);
	drop = (drop > 1.0)? 1.0 : drop;
	return drop;
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    double distance = (position - P).length();
    ray r =ray(P, getDirection(P));
	vec3f d = r.getDirection();
	vec3f result = getColor(P);
	vec3f curP = r.getPosition();
	isect isecP;
	ray newr(curP, d);
	while (scene->intersect(newr, isecP))
	{
		//prevent going beyond this light
		if ((distance -= isecP.t) < RAY_EPSILON) return result;
		//if not transparent return black
		if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0);
		//use current intersection point as new light source
		curP = r.at(isecP.t);
		newr = ray(curP, d);
		result = prod(result, isecP.getMaterial().kt);
	}
	return result;
}

double SpotLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double a = traceUI->getAttenuationConstant();
	double b = traceUI->getAttenuationLinear();
	double c = traceUI->getAttenuationQuadratic();
	double dist = (P - position).length();
	double drop = 1.0/(a + b * dist +c * dist * dist);
	drop = (drop > 1.0)? 1.0 : drop;
	return drop;
}

vec3f SpotLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f SpotLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	vec3f lp = (P - position).normalize();
	double intensity = lp.dot(orientation.normalize());
	double bound = cos(angle * 3.1415926535 / 180);
	int index = 0;
	if (bound < intensity)
	{
		index = 1;
	}
    double distance = (position - P).length();
    ray r =ray(P, getDirection(P));
	vec3f d = r.getDirection();
	vec3f result = getColor(P);
	vec3f curP = r.getPosition();
	isect isecP;
	ray newr(curP, d);
	while (scene->intersect(newr, isecP))
	{
		//prevent going beyond this light
		if ((distance -= isecP.t) < RAY_EPSILON) return result;
		//if not transparent return black
		if (isecP.getMaterial().kt.iszero()) return vec3f(0, 0, 0);
		//use current intersection point as new light source
		curP = r.at(isecP.t);
		newr = ray(curP, d);
		result = prod(result, isecP.getMaterial().kt);
	}
	result *= index;
	return result;
}
