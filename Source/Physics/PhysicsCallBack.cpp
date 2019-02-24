#include "stdafx.h"
#include "Physics/PhysicsCallBack.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include "Physics/PhysicsManager.h"
#include "SoundManager.h"

void PhysicsCallBack::initObjects(physx::PxRigidDynamic *body1, physx::PxRigidDynamic *body2) {
    this->body1 = body1;
    this->body2 = body2;
}

/*
Detects when two actors have collided.
*/
void PhysicsCallBack::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
    std::cout << "Collision detected" << std::endl;
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

            std::cout << "\tactor 0: " << collider << std::endl;
            std::cout << "\tactor 1: " << collided << std::endl;

            // NOTE: Is there something more efficient than an if, else-if chain to do here?

            // If something hits something else, actor0 will always be a dynamic object:
            // - player
            // - bot
            // - rocket
            // So we need to distinguish between them
            if (collider == NAME_PLAYER) {
                // check what the player collided with...
                if (collided == NAME_WALL) {
                    // Play a sound only
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_WORLD);
                } else if (collided == NAME_TRAIL) {
                    // Hit the player
                    // Give points to the player that owns the trail
                } else if (collided == NAME_ROCKET) {
                    // Hit the player
                    // Give poitns to the player that owns the rocket
                } else if (collided == NAME_PLAYER) {
                    // Play a sound only
                    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_HOVERCAR);
                }
            } else if (collider == NAME_BOT) {
            } else if (collider == NAME_ROCKET) {

            }


        }
    }
}
