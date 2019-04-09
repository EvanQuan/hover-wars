#pragma once
#include "stdafx.h"
#include "InteractableEntity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"
#include "DataStructures/SpriteSheetDatabase.h"

// Name: InteractableEntity
// Written by: James Cote
// Description: General Entity for objects that players can interact with.
//        These types of entities can be objects such as pickups or killzones (i.e. flame paths).
//        Idea: these entities, when a player collides with them, will send a message to that player 
//            telling them what happens as a result of this collision. Different messages can be generated
//            by inheriting from this class and defining their own message.
class FlameTrail
    : public InteractableEntity
{
public:
    FlameTrail(int iID, int iOwnerID,
               const vec3* vPosition,
               const vec3* vColor,
               float fHeight, float fWidth);
    virtual ~FlameTrail();

    // Implementation of inherited functionality
    virtual void initialize();
    void update(float fTimeInMilliseconds);

    void spawnFlame(const vec3* vNormal, const vec3* vPosition);

    void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);


private:
    float                                   m_fHeight,
                                            m_fWidth;
    SpriteSheetDatabase::sSpriteSheetInfo   m_sSpriteSheetInfo;
    PhysicsManager*                         m_pPhysXMngr;
    vec3                                    m_vColor;

    // Physics Data References
    struct sReferenceBlock
    {
        float              fDuration;  // Duration for the Flame Trail left
        PxRigidDynamic*    pActorRef;  
    };
    vector<sReferenceBlock> m_pReferenceMap;

    // @Override
    eAbility getAbility() const { return eAbility::ABILITY_TRAIL_ACTIVATE; }
    // @Override
    void handleHovercraftCollision(HovercraftEntity* hit);
};
