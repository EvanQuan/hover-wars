#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/LightingComponent.h"
#include "EntityComponentHeaders/RenderComponent.h"

// Infinitely small point that emits a light source.
//    Managed by the Environment Manager.
class PointLight :
     public Entity
{
public:
    PointLight(int iID, const vec3* vPosition);
    virtual ~PointLight();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);

    // Function to Initialize the Light.
    void initialize(float fPower, const vec3* vColor, bool bStatic, const Material* pMaterial = nullptr, const BoundingBox* pBoundingBox = nullptr, const string& sMeshName = "", float m_fMeshScale = 1.0f );

    // Light Manipulation
    void move(vec3 pMoveVec) { m_vPosition += pMoveVec; m_pLightingComponent->updatePosition(&m_vPosition); }

private:
    // Private Light Copy constructor and assignment operator
    PointLight( const PointLight* newLight );
    PointLight& operator=(const PointLight& pCopy);

    // Private Variables
    vec3 m_pColor;
    Mesh* m_pMesh;
    RenderComponent* m_pRenderComponent;
    LightingComponent* m_pLightingComponent;

};

