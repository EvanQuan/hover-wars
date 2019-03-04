#include "stdafx.h"
#include "Physics/PhysicsCallBack.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include "Physics/PhysicsManager.h"
#include "SoundManager.h"
#include "GameStats.h"
#include "EntityManager.h"

#define TYPE        0
#define SUBTYPE     1
#define OWNER       1
#define GROUND_ID   -1

void PhysicsCallBack::initObjects(physx::PxRigidDynamic *body1, physx::PxRigidDynamic *body2) {
    this->body1 = body1;
    this->body2 = body2;
}

/*
Detects when two actors have collided.

https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/AdvancedCollisionDetection.html
*/
void PhysicsCallBack::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {

    // Generate Pointer to Entity Manager
    EntityManager* m_pEntMngr = ENTITY_MANAGER;

    // std::cout << "Collision detected" << std::endl;
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

        // @NOTE what is this if-statement checking for?
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            // Get each of the two colliding actors
            physx::PxRigidActor* actor0 = pairHeader.actors[0];
            physx::PxRigidActor* actor1 = pairHeader.actors[1];

            // Determine the results by actor names
            const char* collider = actor0->getName();
            const char* collided = actor1->getName();

            // Get Entity IDs
            int iColliderID = (*collider == 'g' ? GROUND_ID : stoi(collider));
            int iCollidedID = (*collided == 'g' ? GROUND_ID : stoi(collided));
#ifndef NDEBUG
            std::cout << "\tactor 0: " << collider << std::endl;
            std::cout << "\tactor 1: " << collided << std::endl;
#endif

            // Simply play sound when collided with ground
            if (GROUND_ID == iCollidedID || GROUND_ID == iColliderID )
                SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_WORLD);
            else    // Tell the Entity Manager to Dispatch the  Collision between the two colliding entities
                m_pEntMngr->dispatchCollision(iColliderID, iCollidedID);
        }
    }
}
