#pragma once
#include "stdafx.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "stdafx.h"

/*
Handles the collisions between objects in the game world.

@author Austin Eaton
@author Evan Quan
*/
class PhysicsCallBack : public physx::PxSimulationEventCallback {
public:
    void initObjects(physx::PxRigidDynamic *body1, physx::PxRigidDynamic *body2);

    // PxSimulationEventCallback functions
    // These are necessary for collision events
    // Reference: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/apireference/files/classPxSimulationEventCallback.html
    void         onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {}
    virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {}
    virtual void onWake(PxActor**, PxU32) {}
    virtual void onSleep(PxActor**, PxU32) {}
    virtual void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
private:
    PxRigidDynamic *body1 = NULL;
    PxRigidDynamic *body2 = NULL;

    std::unordered_map<char, ePlayer> ownerToPlayer =
    {
        {C_OWNER_PLAYER_1,  PLAYER_1},
        {C_OWNER_PLAYER_2,  PLAYER_2},
        {C_OWNER_PLAYER_3,  PLAYER_3},
        {C_OWNER_PLAYER_4,  PLAYER_4},
    };

    std::unordered_map<char, eBot> ownerToBot =
    {
        {C_OWNER_BOT_1,     BOT_1},
        {C_OWNER_BOT_2,     BOT_2},
        {C_OWNER_BOT_3,     BOT_3},
        {C_OWNER_BOT_4,     BOT_4},
    };

    std::unordered_map<char, eHovercraft> ownerToHovercraft =
    {
        {C_OWNER_PLAYER_1,  HOVERCRAFT_PLAYER_1},
        {C_OWNER_PLAYER_2,  HOVERCRAFT_PLAYER_2},
        {C_OWNER_PLAYER_3,  HOVERCRAFT_PLAYER_3},
        {C_OWNER_PLAYER_4,  HOVERCRAFT_PLAYER_4},
        {C_OWNER_BOT_1,     HOVERCRAFT_BOT_1},
        {C_OWNER_BOT_2,     HOVERCRAFT_BOT_2},
        {C_OWNER_BOT_3,     HOVERCRAFT_BOT_3},
        {C_OWNER_BOT_4,     HOVERCRAFT_BOT_4},
    };
};
