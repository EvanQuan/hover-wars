#pragma once
#include "stdafx.h"
#include "Entity.h"
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
class InteractableEntity
    : public Entity
{
public:
    // Constructor / Destructor
    InteractableEntity(int iID, int iOwnerID, const vec3* vPosition, eEntityTypes eIEType);
    virtual ~InteractableEntity();

    // Implementation of inherited functionality
    void update(float fTimeInMilliseconds);
    void handleCollision(Entity* pOther);
    void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;

protected:
    // Protected Variables
    int m_iOwnerID;
    Mesh* m_pBillboardMesh;

    // Entity Components
    RenderComponent* m_pRenderComponent;
    PhysicsComponent* m_pPhysicsComponent;
    AnimationComponent* m_pAnimationComponent;

};
