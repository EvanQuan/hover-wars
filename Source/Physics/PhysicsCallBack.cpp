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
void PhysicsCallBack::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
    std::cout << "triggered" << std::endl;
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

       // if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
       // {
            std::cout << "collision 1: " << pairHeader.actors[0]->getName() << std::endl;
            std::cout << "collision 2: " << pairHeader.actors[1]->getName() << std::endl;
       // }
    }
}
