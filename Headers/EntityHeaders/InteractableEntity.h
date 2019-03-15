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
    InteractableEntity(int iID, int iOwnerID, eHovercraft eOwnerHovercraft, const vec3& vPosition, eInteractType eIEType);
    virtual ~InteractableEntity();

    // Implementation of inherited functionality
    virtual void initialize(const string& sFileName,
                            const ObjectInfo* pObjectProperties,
                            const string& sShaderType,
                            float fScale);
    virtual void update(float fTimeInSeconds);
    virtual void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);
    virtual void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const;
    eInteractType getInteractableType() const { return m_eInteractableType; }

    eHovercraft getOwnerHovercraft() const { return m_eOwnerHovercraft; }

protected:
    // Protected Variables
    int m_iOwnerID; // this may be removed? 

    eHovercraft m_eOwnerHovercraft;
    Mesh* m_pMesh;
    eInteractType m_eInteractableType;

    // Entity Components
    RenderComponent* m_pRenderComponent;
    PhysicsComponent* m_pPhysicsComponent;
    AnimationComponent* m_pAnimationComponent;

};
