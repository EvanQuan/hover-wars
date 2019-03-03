#pragma once
#include "EntityComponentHeaders/EntityComponent.h"
/*
For collisions
*/
#include "Physics/PhysicsManager.h"
#include "Mesh.h"

#define WHEEL_COUNT 4
/**************************************************************
 * Name: PhysicsComponent
 * Written by: James Cote Austin Eaton, Evan Quan
 * Description: Provides Physics functionality for Entities that require it.
 *              Any Entity that requires Physics adds a PhysicsComponent to
 *              their class and initializes it through the Entity Manager on
 *              their initialize function.
 *
 *              Inherit from PxSimulationEventCallback so that it can receive
 *              event call backs necessary for collisions.
\***************************************************************************************/
class PhysicsComponent :
    public EntityComponent
{
public:
    // Constructor/Destructor - Destructor must be virtual for proper deletion through unique_ptrs
    PhysicsComponent(int iEntityID, int iComponentID);
    virtual ~PhysicsComponent();

    // After Initialization, the update function should handle things for the Physics of this Entity.
    //    It's not necessarily necessary for the physics component to push information to the Entity as it updates, 
    //    moreso for it to hold and manage the physics information for the entity while providing functions that allow
    //    the entity to query their physics component for desired information.
    void update(float fTimeDeltaInMilliseconds); // Overloading Parent's virtual update function

    void move(float x, float y);
    void rotatePlayer(float x);
    // Various initialization functions as needed.
    void initializeComponent(bool bStatic, Mesh const* pMeshReference, const ObjectInfo::BoundingBox *bb, glm::vec3 position);
    vec3 getLinearVelocity();
    quat getRotation();
    void flipVehicle();
    void dash(float x, float y);
    void jumpVehicle();
    // this function will allow Entities to retrieve the Transform Matrix required to modify their mesh.
    void getTransformMatrix(mat4* pReturnTransformMatrix);

private:
    bool isInAir;
    /*
    Tracks the last dash to reset the max speed to normal.
    */
    float m_fSecondsSinceLastDash;
    /*
    If true, the max speed is the dash max speed, otherwise the normal max speed
    */
    bool isDashing;
    void releaseAllControls();
    physx::PxVehicleNoDrive *gVehicleNoDrive;
    physx::PxRigidDynamic *body;
    bool m_bStatic;                         // Flag for determining if the Physics Component is Static or Dynamic, I assume this is important and will influence how the Physics component functions under the hood.
    PhysicsManager* m_pPhysicsManager;      // Reference to Physics Manager for calling for any updates necessary.
    mat4 m_pTransformationMatrix;           // Stored Locally, maybe pulled from PhysicsManager on update?

    void setDriveTorque(float torque)
    {
        for (physx::PxU32 wheel = 0; wheel < WHEEL_COUNT; wheel++)
        {
            gVehicleNoDrive->setDriveTorque(wheel, torque);
        }
    }
    void setBrakeTorque(float torque)
    {
        for (physx::PxU32 wheel = 0; wheel < WHEEL_COUNT; wheel++)
        {
            gVehicleNoDrive->setBrakeTorque(wheel, torque);
        }
    }
    void setSteerAngle(float angle)
    {
        for (physx::PxU32 wheel = 0; wheel < WHEEL_COUNT; wheel++)
        {
            gVehicleNoDrive->setBrakeTorque(wheel, angle);
        }
    }
};

