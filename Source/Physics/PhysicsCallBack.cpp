#include "Physics/PhysicsCallBack.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include <iostream>

void PhysicsCallBack::initObjects(physx::PxRigidDynamic *body1, physx::PxRigidDynamic *body2) {
    this->body1 = body1;
    this->body2 = body2;
}

/*
Detects when two actors have collided.
*/
void PhysicsCallBack::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
    std::cout << "triggered" << std::endl;
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

        // @NOTE what is this if-statement checking for?
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            // Get each of the two colliding actors
            physx::PxRigidActor* actor0 = pairHeader.actors[0];
            physx::PxRigidActor* actor1 = pairHeader.actors[1];
            std::cout << "collision 1: " << actor0->getName() << std::endl;
            std::cout << "collision 2: " << actor1->getName() << std::endl;
        }
    }
}
