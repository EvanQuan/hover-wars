#include "EntityComponentHeaders/PhysicsComponent.h"
#include "stdafx.h"
#include <iostream>

/*
Angular momementum.

The greater this value, the faster the maximum turning rate.
*/
#define ANGULAR_MOMENTUM 3.0f
/*
This determines the amount of force applied to the car when movement is intiated.
The greater the force, the faster it will accelerate.

Force : Newtons
*/
#define MOVEMENT_FORCE 40.0f // 10000.0f
/*
This determines the rate of decceleration when the car input movement is in neutral.
A braking force is applied when this is the case to help combat drifting.

NOTE: This may not actually do anything a the moment.

Force : Newtons
*/
#define BRAKE_FORCE 100000.0f // 1000.0f

// Default Constructor: Requires necessary information to pass to Parent Constructor
//      Parameters: iEntityID - ID reference for Entity that initializes this Component.
//                              Necessary for messaging or associating events with this entity 
//                              as needed.
//                  iComponentID - ID reference for this Component to identify it within the
//                                 Entity Manager if needed.

PhysicsComponent::PhysicsComponent(int iEntityID, int iComponentID)
    : EntityComponent(iEntityID, iComponentID)
{
#ifdef _DEBUG
    std::cout << "Physics Component constructor 2 vars" << std::endl;
#endif
    m_bStatic = false;    // Set a default
    m_pPhysicsManager = PHYSICS_MANAGER;    // Grab reference to Physics Manager
    m_pTransformationMatrix = mat4(1.0f);
}
void PhysicsComponent::releaseAllControls()
{
    gVehicleNoDrive->setDriveTorque(0, 0.0f);
    gVehicleNoDrive->setDriveTorque(1, 0.0f);
    gVehicleNoDrive->setDriveTorque(2, 0.0f);
    gVehicleNoDrive->setDriveTorque(3, 0.0f);

    gVehicleNoDrive->setBrakeTorque(0, 0.0f);
    gVehicleNoDrive->setBrakeTorque(1, 0.0f);
    gVehicleNoDrive->setBrakeTorque(2, 0.0f);
    gVehicleNoDrive->setBrakeTorque(3, 0.0f);

    gVehicleNoDrive->setSteerAngle(0, 0.0f);
    gVehicleNoDrive->setSteerAngle(1, 0.0f);
    gVehicleNoDrive->setSteerAngle(2, 0.0f);
    gVehicleNoDrive->setSteerAngle(3, 0.0f);
}
void PhysicsComponent::movePlayer(float x, float y) {   
    if (x != 0 || y != 0) {
        releaseAllControls();
        PxRigidBody *carBody = gVehicleNoDrive->getRigidDynamicActor();
        PxTransform globalTransform = carBody->getGlobalPose();
        PxVec3 vForce = globalTransform.q.rotate(PxVec3(y, 0, x));
        carBody->addForce(vForce * MOVEMENT_FORCE);
        
        float angle = y == 0 ? 0 : -1 * atan(x / y);
        gVehicleNoDrive->setSteerAngle(0, angle);
        gVehicleNoDrive->setSteerAngle(1, angle);
        gVehicleNoDrive->setSteerAngle(2, angle);
        gVehicleNoDrive->setSteerAngle(3, angle);
    }
    else {
        releaseAllControls();
        gVehicleNoDrive->setBrakeTorque(0, BRAKE_FORCE);
        gVehicleNoDrive->setBrakeTorque(1, BRAKE_FORCE);
        gVehicleNoDrive->setBrakeTorque(2, BRAKE_FORCE);
        gVehicleNoDrive->setBrakeTorque(3, BRAKE_FORCE);
    }
}
void PhysicsComponent::rotatePlayer(float x) {
    gVehicleNoDrive->getRigidDynamicActor()->setAngularVelocity(physx::PxVec3(0, -x * ANGULAR_MOMENTUM, 0));
}
// Virtual Destructor, clean up any memory necessary here.
PhysicsComponent::~PhysicsComponent()
{
    /* Not Implemented yet.*/
}

/************************************************************************************\
 * Public Functions                                                                    *
\************************************************************************************/

// Physics Component update functionality. This acts as the bridge between
//    the Physics Manager and the Entity wanting to know about its physics.
//    Maybe this needs to update aspects of the particular physics related to its entity?
//    Maybe this just needs to communicate to the Physics Manager to grab and store updated 
//    information that will be gathered by the Entity when they need it?
void PhysicsComponent::update(float fTimeDeltaInMilliseconds)
{
    // PHYSICSTODO: Provide Component Update functionality here.
}

// Initializes The Physics Component to enable an Entity to have physics for themselves within
//    the scene.
void PhysicsComponent::initializeComponent(bool bStatic, Mesh const* pMeshReference)
{
    // Set up Internal Static qualifier.
    m_bStatic = bStatic;
    gVehicleNoDrive = m_pPhysicsManager->createPlayerEntity();
    body = gVehicleNoDrive->getRigidDynamicActor();

    // PHYSICSTODO: Initialize Component with how the Entity wants it set up.
    //    multiple versions of these Initialze functions may need to be employed to allow
    //    Entities to choose the type of initialization they want.
    //        For example: a Plane probably wnats a Plane bounding box, but a Billboard probably
    //            wants a cylindrical bounding box and will set it up as it needs it.
    //    Make these as general and apparent as possible while providing as much functionality as you can. 
}

// Returns the Rotation Quaternion for the Entity's body.
quat PhysicsComponent::getRotation()
{
    PxQuat pCurrRotation;

    if (nullptr != body)
    {
        pCurrRotation = body->getGlobalPose().q;
    }

    return quat(pCurrRotation.w, pCurrRotation.x, pCurrRotation.y, pCurrRotation.z);
}

void PhysicsComponent::initializeComponent(bool bStatic, Mesh const* pMeshReference, float x, float y, float z, float size)
{
    // Set up Internal Static qualifier.
    m_bStatic = bStatic;
    body = m_pPhysicsManager->createCubeObject(x,y,z,size);

    // PHYSICSTODO: Initialize Component with how the Entity wants it set up.
    //    multiple versions of these Initialze functions may need to be employed to allow
    //    Entities to choose the type of initialization they want.
    //        For example: a Plane probably wnats a Plane bounding box, but a Billboard probably
    //            wants a cylindrical bounding box and will set it up as it needs it.
    //    Make these as general and apparent as possible while providing as much functionality as you can. 
}
// Functionality provided to Entity for getting necessary information about
//     their physics via their physics component.
void PhysicsComponent::getTransformMatrix(mat4* pReturnTransformMatrix)
{
    // Return the Transformation Matrix to the caller, most likely will be the Entity to
    //    update their mesh.
    // Internal Function to swap a PhysX Mat44 to a glm mat4 (column to row-major order)
    if (body != NULL) {
        m_pTransformationMatrix = m_pPhysicsManager->getMat4(body->getGlobalPose());
        // std::cout << "here: " << m_pTransformationMatrix.length() << std::endl;
        //TODO maybe move getMat4 to physicsComponent?

        *pReturnTransformMatrix = m_pTransformationMatrix;
    }
}
glm::vec3 PhysicsComponent::getLinearVelocity() {
    physx::PxVec3 velocity = body->getLinearVelocity();
    return glm::vec3(velocity.x, velocity.y, velocity.z);
}
