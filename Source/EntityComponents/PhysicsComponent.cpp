#include "EntityComponentHeaders/PhysicsComponent.h"
#include "stdafx.h"
#include <iostream>

#define JUMP_FORCE 200000

/*
The maximum speed the player can travel while dashing. This returns back to MAX_SPEED
shortly afterwards.

Speed : meters/second
*/
#define MAX_DASH_SPEED 1000
/*
The amount of time the player can be in MAX_DASH_SPEED after dashing.

Time : seconds
*/
#define DASH_TIME 0.25
/*
Angular momementum.

The greater this value, the faster the maximum turning rate.
*/
#define ANGULAR_MOMENTUM 5.0f // 3.0f
/*
This determines the amount of force applied to the car when movement is intiated.
The greater the force, the faster it will accelerate.

50 @ 9.81

200 @ 300 kg

2000 @ 1000 kg, 50 grav

20000.0f @ 2000kg, 50 grav
Force : Newtons
*/
#ifndef NDEBUG
#define MOVEMENT_FORCE 100000.0f //
#else
#define MOVEMENT_FORCE 1000.0f //
#endif // !1

/*
1000000.0f @ 300 kg
3000000.0f @ 2000 kg 50 g
*/
#define DASH_FORCE 6000000.0f
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
//               iComponentID - ID reference for this Component to identify it within the
//                              Entity Manager if needed.

PhysicsComponent::PhysicsComponent(int iEntityID, int iComponentID)
    : EntityComponent(iEntityID, iComponentID)
{
#ifdef _DEBUG
    std::cout << "Physics Component constructor 2 vars" << std::endl;
#endif
    m_bVehicle = false;    // Set a default
    m_pPhysicsManager = PHYSICS_MANAGER;    // Grab reference to Physics Manager
    m_pTransformationMatrix = mat4(1.0f);

    isDashing = false;
    m_fMaxDashSpeed = MAX_DASH_SPEED;
}

/*
Applies an upward force to the car. Why is this here?
*/
void PhysicsComponent::jumpVehicle() {
    gVehicleNoDrive->getRigidDynamicActor()->addForce(PxVec3(0, JUMP_FORCE, 0));
}
/*
This is never called because there is never a point there the movement input is
completely neutral
*/
void PhysicsComponent::releaseAllControls()
{
    setDriveTorque(0.0f);
    setBrakeTorque(0.0f);
    setSteerAngle(0.0f);
}

/*
Move a player according to x, y coordinates where

                Map view

                  y = 1
                    ^
                    |
    x = -1  <-- hovercraft --> x = 1
                    |
                    v
                  y = -1
*/
void PhysicsComponent::move(float x, float y) {   
    // if ((x != 0 || y != 0) && !isInAir) {
    // if ((x != 0 || y != 0)) {
    // if (!isInAir) {
        releaseAllControls();
        PxTransform globalTransform = body->getGlobalPose();
        PxVec3 vForce = globalTransform.q.rotate(PxVec3(-x, 0, y));
        body->addForce(vForce * MOVEMENT_FORCE);
        
        // TODO find out the angle in a better way
        float angle = y == 0 ? 0 : -1 * atan(x / y);
        setSteerAngle(angle);
    // }
}
PxTransform PhysicsComponent::getGlobalPose() {
    return body->getGlobalPose();
}

void PhysicsComponent::moveGlobal(float x, float y) {
    if ((x != 0 || y != 0)) {
        PxVec3 vForce = PxVec3(y, 0, x);
        body->addForce(vForce * MOVEMENT_FORCE/7); // NOTE: Why are we dividing by 7?

        // TODO find out the angle in a better way
        float angle = y == 0 ? 0 : -1 * atan(x / y);
        setSteerAngle(angle);
    }
}
void PhysicsComponent::dash(float x, float y) {
    // Increase the max speed so that dashing can go faster than normal movement
    setMaxSpeedToDash();
    m_fSecondsSinceLastDash = 0.0f;
    isDashing = true;

    PxTransform globalTransform = body->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(-x, 0, y));
    body->addForce(vForce * DASH_FORCE);

    float angle = y == 0 ? 0 : -1 * atan(x / y);
    setSteerAngle(angle);
}
void PhysicsComponent::rotatePlayer(float x) {
    // if (!isInAir) {
    // TODO Find out why this is a problem? Initially the player is in the air and can't turn?
        gVehicleNoDrive->getRigidDynamicActor()->setAngularVelocity(physx::PxVec3(0, -x * ANGULAR_MOMENTUM, 0));
    // }
}
// Virtual Destructor, clean up any memory necessary here.
PhysicsComponent::~PhysicsComponent()
{
    // Clean up any Rigid Dynamic Objects that were created
    for (unordered_map<string, PxRigidDynamic*>::iterator pIter = m_pDynamicObjects.begin();
        pIter != m_pDynamicObjects.end();
        ++pIter)
        pIter->second->release();
}

/************************************************************************************\
 * Public Functions                                                                    *
\************************************************************************************/

// Physics Component update functionality. This acts as the bridge between
//    the Physics Manager and the Entity wanting to know about its physics.
//    Maybe this needs to update aspects of the particular physics related to its entity?
//    Maybe this just needs to communicate to the Physics Manager to grab and store updated 
//    information that will be gathered by the Entity when they need it?
void PhysicsComponent::update(float fTimeInSeconds)
{
    //PxVec3 vel = body->getLinearVelocity();
    //std::cout << vel.magnitude() << std::endl;
    /*if (vel.magnitude() > MAX_SPEED) {
        //vel.normalize();
        //body->setLinearVelocity(vel * MAX_SPEED);
    }*/
    // gVehicleNoDrive->

    if (m_bVehicle)
    {
        m_fSecondsSinceLastDash += fTimeInSeconds;

        if (isDashing && (m_fSecondsSinceLastDash > DASH_TIME))
        {
            setMaxSpeedToNormal();
        }


        isInAir = PHYSICS_MANAGER->updateCar(gVehicleNoDrive, fTimeInSeconds);
    }
    else
    {
        // Clear Dynamic Objects that are flagged for removal.
        for (vector< string >::iterator pIter = m_pObjectsFlaggedForRemoval.begin();
            pIter != m_pObjectsFlaggedForRemoval.end();
            ++pIter)
            removeInstance(*pIter);

        m_pObjectsFlaggedForRemoval.clear();
    }
    // if (isInAir) {
        // cout << isInAir << endl;
    // }
    // isInAir = true;
}

// TODO
// This will flip the vehicle over, however it will also reset it's x and y rotation
// so it is possible to get wall clipping when calling this method. A better solution is
// desirable.
// TODO
// sync this method to a button press.
void PhysicsComponent::flipVehicle() {
    PxTransform pxTrans = gVehicleNoDrive->getRigidDynamicActor()->getGlobalPose();
    pxTrans.q = PxQuat(PxIdentity);
    gVehicleNoDrive->getRigidDynamicActor()->setGlobalPose(pxTrans);
}
// Initializes The Physics Component to enable an Entity to have physics for themselves within
//    the scene.
void PhysicsComponent::initializeVehicle(const char* sEntityID, bool bStatic, Mesh const* pMeshReference, const ObjectInfo::BoundingBox *bb,glm::vec3 position, float maxNormalSpeed)
{
    // Set up Internal Static qualifier.
    m_bVehicle = bStatic;
    gVehicleNoDrive = m_pPhysicsManager->createHovercraftEntity(sEntityID, position.x, position.y, position.z,bb->vDimensions.x,bb->vDimensions.y, bb->vDimensions.z);
    body = gVehicleNoDrive->getRigidDynamicActor();

    setMaxSpeed(maxNormalSpeed);
}

void PhysicsComponent::initializeRocket(const char* sName, const mat4* m4Transform, const vec3* vVelocity, float fBBLength)
{
    // Generate the Rocket in the Physics Manager
    PxRigidDynamic *pNewRocket = nullptr;
    m_pDynamicObjects.insert(make_pair((sName), pNewRocket)); // Store Rocket internally for management.
    unordered_map<string, PxRigidDynamic*>::iterator pIter = m_pDynamicObjects.find(sName);
    m_pPhysicsManager->createRocketObjects(pIter->first.c_str(), m4Transform, vVelocity, fBBLength, &(pIter->second));

    // Ensure the rocket was created properly.
    assert(nullptr != pIter->second);
}

// Initializes a new Flame Object in the Physics Manager and stores it in the component to manage locally.
void PhysicsComponent::initializeFlame(const char* sName, const vec3* vPosition, float fHeight, float fRadius)
{
    // Generate the Flame in the Physics Manager
    PxRigidDynamic *pNewFlame = nullptr;
    m_pPhysicsManager->createFlameObject(sName, vPosition, fHeight, fRadius, &pNewFlame);

    // Store Flame internally for management.
    assert(nullptr != pNewFlame);
    m_pDynamicObjects.insert(make_pair((sName), pNewFlame));
}

// Remove and unload a DynamicBody from the Physics scene with the given Hashkey.
void PhysicsComponent::removeInstance(string sHashKey)
{
    // Ensure the Instance has been set up first.
    if (!m_bVehicle && (m_pDynamicObjects.find(sHashKey) != m_pDynamicObjects.end()))
    {
        // Release the Instance from the Physics Scene and erase it from the Object List.
        m_pPhysicsManager->removeRigidDynamicObj(m_pDynamicObjects[sHashKey]);
        m_pDynamicObjects.erase(sHashKey);
    }
}

void PhysicsComponent::scaleInstance(string sHashKey, float fScale)
{
    if (!m_bVehicle && (m_pDynamicObjects.find(sHashKey) != m_pDynamicObjects.end()))
    {
        // Scale the Body of the specified Object
        PxScaleRigidActor(*m_pDynamicObjects[sHashKey], fScale);
    }
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

// Get the Transformation Matrix for a specified Dynamic Object at a given hash key
void PhysicsComponent::getTransformMatrix(string sHashKey, mat4* pReturnTransformMatrix)
{
    if (!m_bVehicle && m_pDynamicObjects.find(sHashKey) != m_pDynamicObjects.end())
        *pReturnTransformMatrix = m_pPhysicsManager->getMat4(m_pDynamicObjects[sHashKey]->getGlobalPose());
}
glm::vec3 PhysicsComponent::getPosition() {
    return glm::vec3(body->getGlobalPose().p.x, body->getGlobalPose().p.y, body->getGlobalPose().p.z);
}

glm::vec3 PhysicsComponent::getLinearVelocity() {
    physx::PxVec3 velocity = body->getLinearVelocity();
    return glm::vec3(velocity.x, velocity.y, velocity.z);
}

void PhysicsComponent::getDirectionVector(vec3* vReturnVector)
{
    PxQuat pRotationQuaternion = body->getGlobalPose().q;       // Get the Rotation Quaternion for the current global pose
    PxVec3 vPxReturn = PxVec3(0.0f, 0.0f, 1.0f);                // We Want a forward vector (1 in z-axis) rotated with the global quaternion
    vPxReturn = pRotationQuaternion.rotate(vPxReturn);          // Rotate the Forward Vector
    memcpy(vReturnVector, &vPxReturn, sizeof(vec3));            // Copy PxVec3 to the return Vec3
}

void PhysicsComponent::setMaxSpeed(float maxSpeed)
{
    m_fMaxSpeed = maxSpeed;
    if (!isDashing)
    {
        // Update normal speed if not dashing
        setMaxSpeedToNormal();
    }
}

void PhysicsComponent::setMaxSpeedToNormal()
{
    body->setMaxLinearVelocity(m_fMaxSpeed);
}

void PhysicsComponent::setMaxSpeedToDash()
{
    body->setMaxLinearVelocity(m_fMaxDashSpeed);
}
