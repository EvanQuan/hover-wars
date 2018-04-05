#pragma once
#include "Object3D.h"
#include "MeshManager.h"

//////////////////////////////////////////////////////////////////
// Name: MeshObject.h
// Class: Drawable 3D Object that loads a MeshObject generated from an external source.
//			Uses the TriMesh interface to load the MeshObject
// Written by: James Cote
//////////////////////////////////
class MeshObject :
	public Object3D
{
public:
	~MeshObject();

	// Overloaded Virtual Functions from Object3D
	void draw( const vec3& vCamLookAt, float fMinThreshold, float fMaxThreshold, bool m_bPause );
	string getType() { return "MeshObject"; }
	string getDebugOutput();

	mat4 getFreNetFrames();

private:
	// Private Constructor and Copy Constructor to restrict usage to Object_Factory
	MeshObject( const glm::vec3* pPosition, const string* sFileName, long lID, 
				const string* sTexName, const Anim_Track* pAnimTrack );
	MeshObject( const MeshObject* pCopy );

	// Indices for Faces of MeshObject and Additional Buffer Addresses on the GPU for
	//	Indices and Normals
	Mesh* m_pMesh;
	EdgeBuffer* m_pEdgeBuffer;
	quat m_pQuaternion;

	// Inherited from Parent
	void calculateUVs();

	// Friend Class: Object_Factory to create Meshes.
	friend class Object_Factory;
};

