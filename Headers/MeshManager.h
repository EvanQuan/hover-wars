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
	shared_ptr<Mesh> loadMeshFromFile( const string &sFileName );
	shared_ptr<Mesh> generatePlaneMesh(int fHeight, int fWidth);
	void unloadMesh(shared_ptr<Mesh>& pMeshPtr);
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
	unordered_map<string, shared_ptr<Mesh>> m_pMeshCache;
};