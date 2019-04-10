#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"

// Name: StaticEntity
// Written by: James Cote
// Description: General Entity for static objects in the game world.
//        Static objects are considered objects that don't move and don't interact
//        with anything other than simple physics.
class StaticEntity
    : public Entity
{
public:
    StaticEntity(int iID, const vec3* vPosition);
    virtual ~StaticEntity();

    // Implementation of inherited functionality
    void update(float fTimeInSeconds);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    void loadAsPlane(const vec3* vNormal, int iHeight, int iWidth, const ObjectInfo* pObjectProperties, const string& sShaderType);
    void loadAsSphere(float fRadius, const ObjectInfo* pObjectProperties, const string& sShaderType);
    void loadAsCube(const ObjectInfo* pObjectProperties, const vec3* vDimensions, const string& sShaderType);
    void loadFromFile(const string& sFileName, const ObjectInfo* pObjectProperties, const string& sShaderType, float fScale);

private:
    // Private Copy Constructor and Assignment Operator
    StaticEntity(const StaticEntity& pCopy);
    StaticEntity& operator=(const StaticEntity& pCopy);

    Mesh* m_pMesh; // Contains Vertex information about the mesh.
    RenderComponent* m_pRenderComponent; // Component for handling Rendering of the entity
    PxRigidStatic* m_pPhysicsActor;

    const char* m_sPhysicsComponentName;
};
