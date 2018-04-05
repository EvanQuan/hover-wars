#pragma once
#include "Object3D.h"


class Sphere :
	public Object3D
{
public:
	~Sphere();

	// Overridden draw
	void draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause );

	// Overridden Type Output
	string getType() { return "Sphere"; }

	// Overridden Debug Output
	string getDebugOutput();
	
private:
	// Constructor for Spheres
	Sphere( const vec3* pPosition, float fRadius, long lID, const string* sTexName, const Anim_Track* pAnimTrack );
	Sphere( const Sphere* pNewSphere );	// Protected Copy Constructor.
	void generateMesh();
	void addCarteseanPoint( float fPhi, float fTheta );

	// List of Normals for our mesh
	vector<vec3> m_pNormals;

	// Buffer information for Normals
	GLuint m_iNormalBuffer;
	EdgeBuffer* m_pEdgeBuffer;

	// Private Sphere Variables
	float m_fRadius;

	// Inherited from Parent
	void calculateUVs();

	// Declare Object_Factory as a Friend to be sole progenitor of Spheres
	friend class Object_Factory;
};

