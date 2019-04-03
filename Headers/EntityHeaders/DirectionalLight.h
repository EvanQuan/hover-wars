#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/LightingComponent.h"

// Provides an Interface for a Directional Light Component.
//    the Directional Light simulates a light that is being emitted from a source that is infinitely far away.
//    Possible modifications might be:
//        Simulating a Day/Night change by modifying direction, and color values.
// Written by: James Cote
class DirectionalLight :
     public Entity
{
public:
    DirectionalLight(int iID);
    virtual ~DirectionalLight();

    // Implementation of inherited functionality
    void update(float fTimeInSeconds) {};                                               // Not Implemented
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const {};   // Not Implemented

    // Function to Initialize the Light.
    void initialize( const vec3* vDirection, const vec3* vAmbientColor, const vec3* vDiffuseColor, const vec3* vSpecularColor,
                    float fPosition, float fNearPlane, float fFarPlane, unsigned int iShadowHeight, unsigned int iShadowWidth, float fShadowFrame);

    LightingComponent* getLightingComponent() const { return m_pLightingComponent; }

private:
    // Private Light Copy constructor and assignment operator
    DirectionalLight( const DirectionalLight* newLight );
    DirectionalLight& operator=(const DirectionalLight& pCopy);

    // Private Variables
    LightingComponent* m_pLightingComponent;
};

