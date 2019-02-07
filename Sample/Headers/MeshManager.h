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
	Mesh* loadMeshFromFile( const string &sFileName, vec3 vPosition = vec3(0.f), bool bStaticMesh = false );
	Mesh* generatePlaneMesh(bool bStaticMesh, int iHeight, int iWidth, vec3 vPosition = vec3(0.f), vec3 vNormal = vec3(0.f, 1.f, 0.f));
	Mesh* generateSphereMesh(bool bStaticMesh, float fRadius, vec3 vPosition = vec3(0.f));
	Mesh* generateCubeMesh(bool bStaticMesh, int iHeight, int iWidth, int iDepth, vec3 vPosition = vec3(0.f));
	void unloadAllMeshes();

private:
	// Singleton Implementation
	static MeshManager* pInstance;
	MeshManager();
	MeshManager( const MeshManager& pCopy ) {}
	MeshManager& operator=( const MeshManager& pRHS ) {}
	
	// Private Functions
	bool initializeMesh( Mesh* pMesh, const string& sFileName, vec3 vPosition );

	// Hash Map
	unordered_map<string, unique_ptr<Mesh>> m_pMeshCache;
};