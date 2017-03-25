#include <cmath>
#include <assert.h>

#include "Box.h"

bool intersectionParallel(double A, double B, double C, double D, const ray&r)
{
	vec3f pn = vec3f(A, B, C);
	vec3f d = r.getDirection();
	return (pn.dot(d) < RAY_EPSILON && pn.dot(d) > -RAY_EPSILON);
}

double intersectQuadrilaterals(double A, double B, double C, double D, const ray&r)
{
	vec3f pn = vec3f(A, B, C);
	vec3f d = r.getDirection();
	vec3f p = r.getPosition();
	double t = -(pn.dot(p) + D)/(pn.dot(d));
	return t;
}
bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	
	// check intersection with x =0.5
	double t = 100000000000.0;
	bool intersect = false;
	if (!intersectionParallel(1, 0, 0, -0.5, r))
	{
		double cur = intersectQuadrilaterals(1, 0, 0, -0.5, r);
		if (cur > RAY_EPSILON)
		{	
			vec3f curPos = r.at(cur);
			if (curPos[1] < 0.5 && curPos[1] > -0.5 && curPos[2] < 0.5 && curPos[2] > -0.5)
			{
				t = (cur < t)? cur:t;
				intersect = true;
				i.N = vec3f(1, 0, 0);
			}
		}
	}
	if (!intersectionParallel(1, 0, 0, 0.5, r))
	{
		double cur = intersectQuadrilaterals(1, 0, 0, 0.5, r);
		if (cur > RAY_EPSILON)
		{
			vec3f curPos = r.at(cur);
			if (curPos[1] < 0.5 && curPos[1] > -0.5 && curPos[2] < 0.5 && curPos[2] > -0.5)
			{
				t = (cur < t)? cur:t;
				intersect = true;
				i.N = vec3f(1, 0, 0);
			}
		}
	}
	if (!intersectionParallel(0, 1, 0, -0.5, r))
	{
		double cur = intersectQuadrilaterals(0, 1, 0, -0.5, r);
		if (cur > RAY_EPSILON)
		{
			vec3f curPos = r.at(cur);
			if (curPos[0] < 0.5 && curPos[0] > -0.5 && curPos[2] < 0.5 && curPos[2] > -0.5)
			{
				t = (cur < t)? cur:t;
				intersect = true;
				i.N = vec3f(0, 1, 0);
			}
		}
	}
	if (!intersectionParallel(0, 1, 0, 0.5, r))
	{
		double cur = intersectQuadrilaterals(0, 1, 0, 0.5, r);
		if (cur > RAY_EPSILON)
		{
			vec3f curPos = r.at(cur);
			if (curPos[0] < 0.5 && curPos[0] > -0.5 && curPos[2] < 0.5 && curPos[2] > -0.5)
			{
				t = (cur < t)? cur:t;
				intersect = true;
				i.N = vec3f(0, 1, 0);
			}
		}
	}
	if (!intersectionParallel(0, 0, 1, -0.5, r))
	{
		double cur = intersectQuadrilaterals(0, 0, 1, -0.5, r);
		if (cur > RAY_EPSILON)
		{
			vec3f curPos = r.at(cur);
			if (curPos[1] < 0.5 && curPos[1] > -0.5 && curPos[0] < 0.5 && curPos[0] > -0.5)
			{
				t = (cur < t)? cur:t;
				intersect = true;
				i.N = vec3f(0, 0, 1);
			}
		}
	}
	if (!intersectionParallel(0, 0, 1, 0.5, r))
	{
		double cur = intersectQuadrilaterals(0, 0, 1, 0.5, r);
		if (cur > RAY_EPSILON)
		{
			vec3f curPos = r.at(cur);
			if (curPos[1] < 0.5 && curPos[1] > -0.5 && curPos[0] < 0.5 && curPos[0] > -0.5)
			{
				t = (cur < t)? cur:t;
				intersect = true;
				i.N = vec3f(0, 0, 1);
			}
		}
	}

	if (intersect)
	{
		i.obj = this;
		i.t = t;
	}
	return intersect;
}

