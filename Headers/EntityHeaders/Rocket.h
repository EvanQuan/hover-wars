#pragma once
#include "stdafx.h"
#include "InteractableEntity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"

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
    Rocket(int iID, int iOwnerID, const vec3* vPosition);
    virtual ~Rocket();

    // Implementation of inherited functionality
    void initialize() { /*Not Implemented yet*/ }
    void update(float fTimeInMilliseconds);
    void handleCollision(Entity* pOther);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

private:
    Mesh* m_pMesh;
    ObjectInfo m_pObjectInfo;

    vec3 m_vDimensions;

    RenderComponent* m_pRenderComponent; // Component for handling Rendering of the entity
    // PhysicsComponent* m_pPhysicsComponent; // Component for handling Physics for the Entity
    AnimationComponent* m_pAnimationComponent;
};
