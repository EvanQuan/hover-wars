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
#include <sstream>

#define TYPE        0
#define SUBTYPE     1
#define OWNER       1
#define GROUND_ID   -1

void PhysicsCallBack::initObjects(physx::PxRigidDynamic *body1, physx::PxRigidDynamic *body2) {
    this->m_pBody1 = body1;
    this->m_pBody2 = body2;
}

/*
Detects when two actors have collided.

https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/AdvancedCollisionDetection.html
*/
void PhysicsCallBack::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {

    // Generate Pointer to Entity Manager
    EntityManager* m_pEntMngr = ENTITY_MANAGER;
    int iColliderID, iCollidedID;

    // std::cout << "Collision detected" << std::endl;
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

        // @NOTE what is this if-statement checking for?
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            // Determine the results by actor names
            string sCollider = pairHeader.actors[0]->getName(); // ""
            string sCollided = pairHeader.actors[1]->getName(); // "14"

            // temporary fix
            if (!sCollider.empty() && !sCollided.empty())
            {

                // Parse Names
                istringstream HitterSS(sCollider);
                vector<string> HitterResults(istream_iterator<string>{HitterSS},
                    istream_iterator<string>());
                // istringstream VictimSS(sCollided);
                istringstream VictimSS(sCollided);
                vector<string> VictimResults(istream_iterator<string>{VictimSS},
                    istream_iterator<string>());

                // Get Entity IDs
                // The ground has its own special ID since its not treated the same
                // as the other entities
                // @NOTE this crashes when sCollider == ""
                try {
                    if (!HitterResults.empty() && !VictimResults.empty())
                    {
                        iColliderID = stoi(HitterResults.front());
                        iCollidedID = stoi(VictimResults.front());
                    }
                    else
                        continue; // Skip this collision if it's unreadable.
                }
                catch (invalid_argument e)
                {
                    continue; // Skip this collision if it's unreadable
                }
                unsigned int iColliderMsg = 0, iCollidedMsg = 0;

#ifndef NDEBUG
                // Player 1: 11
                // Bot 1: 14
                // std::cout << "\tactor 0: " << HitterResults.front() << std::endl;
                // std::cout << "\tactor 1: " << VictimResults.front() << std::endl;
#endif
                // Catch Messages if available.
                if (HitterResults.size() > 1)
                    iColliderMsg = stoi(HitterResults[1]);
                if (VictimResults.size() > 1)
                    iCollidedMsg = stoi(VictimResults[1]);

                // Pass information to Entity Manager to handle collision
                m_pEntMngr->dispatchCollision(iColliderID, iCollidedID, iColliderMsg, iCollidedMsg);
            }
        }
    }
}
