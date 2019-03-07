#pragma once
#include "stdafx.h"
#include "InteractableEntity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"


// Forward Declarations
class EmitterEngine;

// Name: InteractableEntity
// Written by: James Cote
// Description: General Entity for objects that players can interact with.
//        These types of entities can be objects such as pickups or killzones (i.e. flame paths).
//        Idea: these entities, when a player collides with them, will send a message to that player 
//            telling them what happens as a result of this collision. Different messages can be generated
//            by inheriting from this class and defining their own message.
class Rocket
    : public InteractableEntity
{
public:
    Rocket(int iID, int iOwnerID);
    virtual ~Rocket();

    // Implementation of inherited functionality
    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale);
    void update(float fTimeInMilliseconds);
    void handleCollision(Entity* pOther);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    // Rocket Functionality
    void launchRocket(const vec3* vPosition, const vec3* vDirection, float fDuration);

private:
    float           m_fRocketDuration;
    EmitterEngine*  m_pEmitterEngine;
};
