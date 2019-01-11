#pragma once
#include "stdafx.h"
#include <unordered_map>
#include "Mesh.h"

class MeshManager
{
public:
	// creates/returns singleton instance of MeshManager
	static MeshManager* getInstance();
	~MeshManager();

	// Methods:
	Mesh* loadMeshFromFile( const string &sFileName );
	Mesh* generatePlaneMesh(int fHeight, int fWidth, vec3 vPosition = vec3(0.f), vec3 vNormal = vec3(0.f, 1.f, 0.f));
	void unloadAllMeshes();

private:
	// Singleton Implementation
	static MeshManager* pInstance;
	MeshManager();
	MeshManager( const MeshManager& pCopy ) {}
	MeshManager& operator=( const MeshManager& pRHS ) {}
	
	// Private Functions
	bool initializeMesh( Mesh* pMesh, const string& sFileName );

	// Hash Map
	unordered_map<string, unique_ptr<Mesh>> m_pMeshCache;
};