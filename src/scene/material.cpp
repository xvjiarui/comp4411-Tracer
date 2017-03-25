// extern TraceUI* traceUI;
#include "ray.h"
#include "material.h"
#include "light.h"
#include "../ui/TraceUI.h"
#include "scene.h"
#include <cmath>
extern TraceUI* traceUI;

typedef list<Light*>::iterator 			liter;
typedef list<Light*>::const_iterator 	cliter;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	vec3f I = ke + ka * traceUI->getAmbientLight();
	for (cliter li = scene->beginLights(); li != scene->endLights(); ++li)
	{
		vec3f atten = (*li)->distanceAttenuation(r.at(i.t)) * (*li)->shadowAttenuation(r.at(i.t));
		double diffuse_coef = (i.N).dot((*li)->getDirection(r.at(i.t)));
		diffuse_coef = (diffuse_coef > 0)? diffuse_coef : 0;
		vec3f diffuse_term = kd * diffuse_coef;
		vec3f reflection = (2 * ((*li)->getDirection(r.at(i.t)).normalize().dot(i.N) * i.N) - (*li)->getDirection(r.at(i.t)).normalize()).normalize();
		vec3f view = - r.getDirection().normalize();
		double specular_coef = reflection.dot(view);
		specular_coef = (specular_coef > 0)? specular_coef : 0;
		specular_coef = pow(specular_coef, shininess * 128);
		vec3f specular_term = ks * specular_coef;
		I += prod(atten ,diffuse_term + specular_term);
	}
	I = I.clamp();

	return I;
}
