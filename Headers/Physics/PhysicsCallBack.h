#pragma once
#include "stdafx.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "PxFiltering.h"
#include "PxSimulationEventCallback.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "stdafx.h"

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
};
