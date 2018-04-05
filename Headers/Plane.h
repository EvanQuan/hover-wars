#pragma once
#include "Object3D.h"
class Plane :
	public Object3D
{
public:
	~Plane();

	// Overridden intersect function
	void draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause );

	// Overridden Type Output
	string getType() { return "Plane"; }
	
	// Overridden Debug Output
	string getDebugOutput();

private:
	Plane( const vec3* pPosition, const vector<vec3>* pCorners, long lID, const string* sTexName, bool bUseEB, const Anim_Track* pAnimTrack );
	Plane( const Plane* pNewPlane );  // Protected Copy Constructor

	// Normal of the Plane.
	vec3 m_pNormal;	
	EdgeBuffer* m_pEdgeBuffer;

	// Inherited from Parent
	void calculateUVs();

	// Declare Object_Factory as a Friend to be sole progenitor of Planes
	friend class Object_Factory;
};

