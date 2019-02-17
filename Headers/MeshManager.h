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
    Mesh* loadMeshFromFile( const string &sFileName, const ObjectInfo* pObjectProperties, float fScale = 1.0f, bool bStaticMesh = false);
    Mesh* generatePlaneMesh(bool bStaticMesh, int iHeight, int iWidth, const ObjectInfo* pObjectProperties, vec3 vNormal = vec3(0.f, 1.f, 0.f));
    Mesh* generateSphereMesh(bool bStaticMesh, float fRadius, const ObjectInfo* pObjectProperties);
    Mesh* generateCubeMesh(bool bStaticMesh, float fHeight, float fWidth, float fDepth, const ObjectInfo* pObjectProperties);
    Mesh* generateBillboardMesh(const ObjectInfo* pObjectProperties, const void* pOwnerHandle );
    void unloadAllMeshes();

private:
    // Singleton Implementation
    static MeshManager* pInstance;
    MeshManager();
    MeshManager( const MeshManager& pCopy ) {}
    MeshManager& operator=( const MeshManager& pRHS ) {}

    // Private Functions
    string materialToString( const ObjectInfo::Material* sMaterial );

    // Hash Map
    unordered_map<string, unique_ptr<Mesh>> m_pMeshCache;
};
