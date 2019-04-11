#include "EntityComponentHeaders/PhysicsComponent.h"
#include "stdafx.h"

#define JUMP_FORCE 200000

/*
The maximum speed the player can travel while dashing. This returns back to MAX_SPEED
shortly afterwards.

Speed : meters/second
*/
#define MAX_DASH_SPEED 100 // 1000
/*
The amount of time the player can be in MAX_DASH_SPEED after dashing.

Time : seconds
*/
#define DASH_TIME 0.25
/*
Angular momementum.

The greater this value, the faster the maximum turning rate.
*/
#define ANGULAR_MOMENTUM_MULTIPLIER 2.4f // 5.0f
#define ANGULAR_MOMENTUM_EXPONENT 2.0f // 5.0f
/*
This determines the amount of force applied to the car when movement is intiated.
The greater the force, the faster it will accelerate.

50 @ 9.81

200 @ 300 kg

2000 @ 1000 kg, 50 grav

20000.0f @ 2000kg, 50 grav
Force : Newtons
*/

#define MOVEMENT_FORCE 30000000.0f // 100000 // 1000

/*
1000000.0f @ 300 kg
3000000.0f @ 2000 kg 50 g
*/
#define DASH_FORCE 6000000.0f
#define PUSH_FORCE 6000000.0f
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
    m_bVehicle = false;    // Set a default
    m_pPhysicsManager = PHYSICS_MANAGER;    // Grab reference to Physics Manager
    m_pTransformationMatrix = mat4(1.0f);

    m_bIsDashing = false;
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
        body->addForce(vForce * MOVEMENT_FORCE * lastDeltaTime);
        
        // TODO find out the angle in a better way
        float angle = y == 0 ? 0 : -1 * atan(x / y);
        setSteerAngle(angle);
    // }
}
PxTransform PhysicsComponent::getGlobalPose() {
    return body->getGlobalPose();
}
void PhysicsComponent::setGlobalPos(PxTransform trans) {
    body->setGlobalPose(trans);
}
void PhysicsComponent::moveGlobal(float x, float y) {
    PxVec3 vForce = PxVec3(y, 0, x);
    body->clearForce();
    body->addForce(vForce * MOVEMENT_FORCE); // NOTE: Why are we dividing by 7?

    // TODO find out the angle in a better way
    float angle = y == 0 ? 0 : -1 * atan(x / y);
    setSteerAngle(angle);
}
void PhysicsComponent::setPosition(vec2 pos) {
    PxTransform trans = body->getGlobalPose();
    trans.p = PxVec3(pos.x, trans.p.y, pos.y);
    setGlobalPos(trans);
}
void PhysicsComponent::dash(float x, float y) {
    // Increase the max speed so that dashing can go faster than normal movement
    setMaxSpeedToDash();
    m_fSecondsSinceLastDash = 0.0f;
    m_bIsDashing = true;

    push(x, y, DASH_FORCE);
}

void PhysicsComponent::push(float x, float y)
{
    push(x, y, PUSH_FORCE);
}

void PhysicsComponent::push(float x, float y, float force)
{
    PxTransform globalTransform = body->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(-x, 0, y));
    body->addForce(vForce * force);

    float angle = y == 0 ? 0 : -1 * atan(x / y);
    setSteerAngle(angle);
}
void PhysicsComponent::rotatePlayer(float x) {
    // Small joystick movements result in small turning values to make aiming easier.
    // Large joystick movements result in large turning values to make drastic turns more responsive.
    // By making the growth exponential instad of linear, this further helps both types of joystick movement.
    float angularMomentum = (x > 0 ? -1 : 1) * std::abs(std::pow(x * ANGULAR_MOMENTUM_MULTIPLIER, ANGULAR_MOMENTUM_EXPONENT));
    // float angularMomentum = -x * 4;
    // cout << "angularMomentum: " << angularMomentum << endl;
    gVehicleNoDrive->getRigidDynamicActor()->setAngularVelocity(physx::PxVec3(0, angularMomentum, 0));
}
// Virtual Destructor, clean up any memory necessary here.
PhysicsComponent::~PhysicsComponent()
{
    // Clean up any Rigid Dynamic Objects that were created
    for (unordered_map<string, PxRigidDynamic*>::iterator pIter = m_pDynamicObjects.begin();
        pIter != m_pDynamicObjects.end();
        ++pIter)
        m_pPhysicsManager->removeRigidActor(pIter->second);
    m_pDynamicObjects.clear();
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
    lastDeltaTime = fTimeInSeconds;
    if (m_bVehicle)
    {
        m_fSecondsSinceLastDash += fTimeInSeconds;

        if (m_bIsDashing && (m_fSecondsSinceLastDash > DASH_TIME))
        {
            setMaxSpeedToNormal();
        }


        // isInAir = PHYSICS_MANAGER->updateCar(gVehicleNoDrive, fTimeInSeconds);
        PHYSICS_MANAGER->updateCar(gVehicleNoDrive, fTimeInSeconds);
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
void PhysicsComponent::initializeVehicle(const char* sEntityID,
                                         bool bStatic,
                                         const ObjectInfo::BoundingBox *bb,
                                         glm::vec3 position,
                                         float maxNormalSpeed)
{
    // Set up Internal Static qualifier.
    m_bVehicle = bStatic;
    gVehicleNoDrive = m_pPhysicsManager->createHovercraftEntity(sEntityID,
                                        position.x, position.y, position.z,
                                        bb->vDimensions.x,bb->vDimensions.y, bb->vDimensions.z);
    body = gVehicleNoDrive->getRigidDynamicActor();

    if (nullptr == body) {
        cout << "PhysicsComponent::initializeVehicle() failed to initialize" << endl;
    }

    setMaxSpeed(maxNormalSpeed);
}

void PhysicsComponent::initializeRocket(const char* sName,
                                        const mat4* m4Transform,
                                        const vec3* vVelocity,
                                        float fBBLength)
{
    // Generate the Rocket in the Physics Manager
    PxRigidDynamic* pNewBody = nullptr;
    m_pDynamicObjects.insert(make_pair((sName), pNewBody));
    unordered_map<string, PxRigidDynamic*>::iterator pIter = m_pDynamicObjects.find(sName);
    m_pPhysicsManager->createRocketObjects(pIter->first.c_str(),
                                           m4Transform, vVelocity,
                                           fBBLength,
                                           &(pIter->second));

    // Ensure the rocket was created properly.
    assert(nullptr != pIter->second);
}

// Remove and unload a DynamicBody from the Physics scene with the given Hashkey.
void PhysicsComponent::removeInstance(string sHashKey)
{
    // Ensure the Instance has been set up first.
    if (!m_bVehicle && (m_pDynamicObjects.find(sHashKey) != m_pDynamicObjects.end()))
    {
        // Release the Instance from the Physics Scene and erase it from the Object List.
        m_pPhysicsManager->removeRigidActor(m_pDynamicObjects[sHashKey]);
        m_pDynamicObjects.erase(sHashKey);
    }
}

// Flag an object for removal
//  Since the collision call back is on a separate thread, it's not possible to remove the object during the collision
//  call back. Flag it for removal and remove it outside of the callback thread.
void PhysicsComponent::flagForRemoval(string sHashKey)
{
    // Disable simulation
    m_pObjectsFlaggedForRemoval.push_back(sHashKey);
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

    // if (nullptr != body)
    // {
        pCurrRotation = body->getGlobalPose().q;
    // }
    return quat(pCurrRotation.w, pCurrRotation.x, pCurrRotation.y, pCurrRotation.z);
}

// Functionality provided to Entity for getting necessary information about
//     their physics via their physics component.
void PhysicsComponent::getTransformMatrix(mat4* pReturnTransformMatrix)
{
    // Return the Transformation Matrix to the caller, most likely will be the Entity to
    //    update their mesh.
    // Internal Function to swap a PhysX Mat44 to a glm mat4 (column to row-major order)
    // if (nullptr != body) {
        m_pTransformationMatrix = m_pPhysicsManager->getMat4(body->getGlobalPose());
        // std::cout << "here: " << m_pTransformationMatrix.length() << std::endl;
        //TODO maybe move getMat4 to physicsComponent?

        *pReturnTransformMatrix = m_pTransformationMatrix;
    // }
}

// Get the Transformation Matrix for a specified Dynamic Object at a given hash key
void PhysicsComponent::getTransformMatrix(string sHashKey, mat4* pReturnTransformMatrix)
{
    if (!m_bVehicle && m_pDynamicObjects.find(sHashKey) != m_pDynamicObjects.end())
        *pReturnTransformMatrix = m_pPhysicsManager->getMat4(m_pDynamicObjects[sHashKey]->getGlobalPose());
}
glm::vec3 PhysicsComponent::getPosition() {
    physx::PxVec3 position = body->getGlobalPose().p;
    return glm::vec3(position.x, position.y, position.z);
}

glm::vec3 PhysicsComponent::getLinearVelocity() {
    // NOTE: crashes here at game restart. Body is nullptr?
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
    if (!m_bIsDashing)
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
