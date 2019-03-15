#pragma once
#include "stdafx.h"
#include "InteractableEntity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"


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
    Rocket(int iID, int iOwnerID, eHovercraft eOwnerHovercraft);
    virtual ~Rocket();

    // Implementation of inherited functionality
    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale);
    void update(float fTimeInSeconds);
    void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

    // Rocket Functionality
    void launchRocket(const mat4* m4InitialTransform, const vec3* vVelocity, float fBBLength);

private:
    EmitterEngine*                          m_pEmitterEngine;
    unordered_map<string, unsigned int>     m_pReferenceMap;
};
