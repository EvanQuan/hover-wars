#include "Physics/PhysicsCallBack.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include "Physics/PhysicsManager.h"
#include "SoundManager.h"
#include <iostream>

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
            const char* actor0name = actor0->getName();
            const char* actor1name = actor1->getName();

            std::cout << "\tactor 0: " << actor0name << std::endl;
            std::cout << "\tactor 1: " << actor1name << std::endl;

            // If a vehicle hits the world:
            //  actor0 : vehicle
            //  actor1 : cube
            if (actor0name == NAME_VEHICLE && actor1name == NAME_CUBE) {
                SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_IMPACT_WORLD);
            }


        }
    }
}
