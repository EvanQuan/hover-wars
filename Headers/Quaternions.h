#pragma once
/************\
 * Includes *
\************/
#include <glm/glm.hpp>

using namespace glm;

/***********\
 * Defines *
\***********/
#define RAD 57.29577951
#define DegToRad(angle) ((angle)/RAD)

// Quaternion Struct
struct Quaternion
{
	float s;
	vec3 v;
};

// Make a quaternion out of a scalar s and vector v
Quaternion operator,( float s, vec3 v )
{
	Quaternion q;
	q.s = s;
	q.v = v;
	return q;
}

// Conjugate to essentially inverse a Quaternion
Quaternion Conjugate( Quaternion q )
{
	q.v = -q.v;
	return q;
}

// Add two quaternions together
Quaternion operator+( Quaternion q1, Quaternion q2 )
{
	return ((q1.s + q2.s), (q1.v + q2.v));
}

// Multiply a scalar by a quaternion (for completeness)
Quaternion operator*( float u, Quaternion q )
{
	return (u*q.s, u*q.v);
}

// Multiply 2 quaternions together.
Quaternion operator*( Quaternion q1, Quaternion q2 )
{
	return ((q1.s*q2.s - dot( q1.v, q2.v )), (q1.s*q2.v + q2.s*q1.v + cross( q1.v, q2.v )));
}