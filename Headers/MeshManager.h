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
	Mesh* loadMesh( const string &sFileName, long lID );
	void unloadMesh( const string &sFileName, long lID );
private:
	// Singleton Implementation
	static MeshManager* pInstance;
	MeshManager();
	MeshManager( const MeshManager& pCopy ) {}
	MeshManager& operator=( const MeshManager& pRHS ) {}
	
	// MeshContainer structure
	struct MeshContainer
	{
		Mesh* pMesh;					// Mesh Object Reference
		vector<long> lUserIDs;			// List of Users
	};

	// Private Functions
	bool initializeMesh( Mesh* pMesh, const string& sFileName );

	// Hash Map
	unordered_map<string, MeshContainer> m_pMeshCache;
};