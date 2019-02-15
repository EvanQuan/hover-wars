/************
 * Includes *
\************/
#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "Physics/PhysicsManager.h"
#include "vehicle/PxVehicleUtil.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include <iostream>
#include <vector>

/***********\
 * DEFINES *
\***********/
#define PVD_HOST "127.0.0.1"
#define UPDATE_TIME_IN_SECONDS (1.0f / 60.0f) // Physics should update every 1/60th of a second regardless of the game's framerate.

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
#define MOVEMENT_FORCE 20000.0f // 10000.0f
/*
This determines the rate of decceleration when the car input movement is in neutral.
A braking force is applied when this is the case to help combat drifting.

NOTE: This may not actually do anything a the moment.

Force : Newtons
*/
#define BRAKE_FORCE 100000.0f // 1000.0f

/*
Hovercraft material properties
*/
/*
Coefficient of static friction

This determines the threshold for a force to begin moving the car from neutral
(stantionary).
We want this to be relatively small or even 0 since whenever we apply a force
to the car, we typically would want it to move.
*/
#define STATIC_FRICTION 0.0f // 0.35f
/*
Coefficient of dynamic friction

This determines the resistance to force when the car is already moving.
We want this to be high enough to prevent infinite drifting, and so players can
stop and change directions in a reasonably responsive manner.
We want this to be low enough that there some amount of drifting can be done to
make it feel like a hovercraft.
*/
#define DYNAMIC_FRICTION 1.00f // 0.35f
/*
Car Restitution

Affects how elastic collisions are. The greater this value, the more colliding
objects bounce away.

This should be relatively high to make car collisions satisfying.
*/
#define CAR_RESTITUTION 0.2f
/*
Acceleration of gravity downwards

This affects how "floaty" objects feel when they are in the air.
Real world value is -9.81
Typically in games, this value is much greater than the real world value.

Acceleration : m/s^2
*/
#define GRAVITY -9.81f
/*
This affects the momentum of the vehicle.
The greater it is, the slow the car will take to accelerate, either from
neutral or in changing directions. To increase player input responsiveness,
decrease this.
The greater this value, the less responsive cars will be to forces such as
explosions or collisions.

Mass : kilograms
*/
#define CHASSIS_MASS = 1000.0f


/****************************************************************************\
 * Singleton Implementation                                                    *
\****************************************************************************/

// Initializing Static Physics Manager Singleton 
PhysicsManager* PhysicsManager::m_pInstance = nullptr;

// Get Instance Function for fetching singleton instance.
PhysicsManager* PhysicsManager::getInstance()
{
    // If Instance hasn't been initialized yet; initialize the instance first.
    if (nullptr == m_pInstance)
        m_pInstance = new PhysicsManager();

    return m_pInstance;
}
bool hasStarted = false;
// Default Constructor for Physics Manager
//    Initializes all default settings for Physics Manager
PhysicsManager::PhysicsManager()
{
    // basic variable init
    gFoundation = NULL;
    gPhysics = NULL;
    gDispatcher = NULL;
    gScene = NULL;
    manager = NULL;
    gMaterial = NULL;
    
    gPvd = NULL;
    stackZ = -3.0f;// distance between stacks, Only used for stack demo creation
    /*Left these here to show change made by moving these to PhysicsManager Private Variables*/
}

// PhysicsManager Destructor -
//    Cleans up all Physics and destructs any initialized heap memory 
//    used by PhysicsManager
PhysicsManager::~PhysicsManager()
{
    // do soft clean up to ensure nothing is missed.
    cleanupPhysics();

    // PHYSICSTODO: finish any remaining final clean up here:
}

snippetvehicle::VehicleDesc PhysicsManager::initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const PxF32 chassisMass = 300.0f;
    const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
    const PxVec3 chassisMOI
    ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
        (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
        (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
    const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*1.5f + 0.65f, 0.25f);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass = 20.0f;
    const PxF32 wheelRadius = 0.5f;
    const PxF32 wheelWidth = 0.4f;
    const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
    const PxU32 nbWheels = 4;

    snippetvehicle::VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = gMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = gMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
}

void PhysicsManager::releaseAllControls()
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

/****************************************************************************\
 * Public Functions                                                            *
\****************************************************************************/

// Name: Update
// Description: Provides Frame by Frame update functionality for Physics.
//        Updates on a different interval than standard framerate.
void PhysicsManager::update(float fTimeDelta)
{
    // Add the next delta to the loop
    m_fTimeSinceLastUpdate += fTimeDelta;
    // Increment Time since last update and check to see if an update is ready.
    //    if an update is ready, decrement the internal timer to reset it accurately
    //    then perform the update.
    while (m_fTimeSinceLastUpdate >= UPDATE_TIME_IN_SECONDS)
    {
        // Don't reset to 0.0f as it's probably not at the Update Time Exactly.
        //    this preserves an accuracy w.r.t. the game timer.
        m_fTimeSinceLastUpdate -= UPDATE_TIME_IN_SECONDS;

        // Step Physics at 1/60th of a second.
        stepPhysics(); 
    }
}

// Initialization of Physics will happen here, this will be called in main, immediately
//    after constructing the PhysicsManager Instance.
void PhysicsManager::initPhysics(bool interactive)
{
    if (gScene != NULL) {
        return;
    }
    // For my private variables, I append the 'm_' to the front to say, "This is my variable"
    //    "my" referring to the namespace of the function: PhysicsManager. I also put the small case
    //    character in front of the Variable name to signify the type of variable it is, 
    //    'b' for bool, 'i' for int, 'p' for pointer, etc.
    m_bInteractive = interactive;    // Storing this boolean internally to set up
                                    // physics to persistantly be this way. Maybe 
                                    // this needs to be designed differently?

    // Comment each of these lines, tell us what each function is doing and why it is necessary.
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
#ifdef _DEBUG
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, GRAVITY, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    gMaterial = gPhysics->createMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, CAR_RESTITUTION);

    gCook = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
    if (!gCook)
        std::cout << ("PxCreateCooking failed!") << std::endl;


    //////////////////////////////////////////////////////////////////////////////////////////////////


    bool resultVehicle = PxInitVehicleSDK(*gPhysics);
    PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(-1, 0, 0));
    PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    gVehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
    gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = snippetvehicle::createFrictionPairs(gMaterial);

    //Create a plane to drive on.
    PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
    gGroundPlane = snippetvehicle::createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
    gScene->addActor(*gGroundPlane);

    





    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //manager = PxCreateControllerManager(*gScene);
    //PxBoxControllerDesc desc = PxBoxControllerDesc();
    //desc.setToDefault();
    //desc.position = PxExtendedVec3(1,1,1);
    //desc.volumeGrowth = 5.0f;
    //desc.halfForwardExtent = 1;
    //desc.halfHeight = 1;
    //desc.halfSideExtent = 1;
    //desc.stepOffset = 0.5f;
    //std::cout << desc.isValid() << std::endl;
    //PxController* c = manager->createController(desc);
    //std::cout << c << std::endl;
    //initWall(gPhysics);
    //for (PxU32 i = 0; i < 1; i++)
    //    createStack(PxTransform(PxVec3(0, 0, stackZ += 3.0f)), 2, 0.1f);

    //createDynamic(PxTransform(PxVec3(0, 4, 1)), PxSphereGeometry(1), PxVec3(0, -1, -1));
//createDynamic(PxTransform(PxVec3(0, 4, 0)), PxSphereGeometry(1), PxVec3(0, -1, 0));
}
void PhysicsManager::forwardKey() {
    if (currentState != 1) {
        releaseAllControls();
        //startAccelerateForwardsMode();
        currentState = 1;
    }
}
void PhysicsManager::movePlayer(int entityID, float x, float y) {
    /*if (x <0.1 && y <0.1 && y> -0.1 && x > -0.1) {
        std::cout << "here" << std::endl;
        gVehicleNoDrive->setBrakeTorque(0, 1000.0f);
        gVehicleNoDrive->setBrakeTorque(1, 1000.0f);
        gVehicleNoDrive->setBrakeTorque(2, 1000.0f);
        gVehicleNoDrive->setBrakeTorque(3, 1000.0f);
    }else
    {*/
        float angle =-1* atan(x/y);
        float distance = sqrt(x*x + y*y);
        if (y > 0) {
            distance = -distance;
        }
        if (distance > 1) {
            distance = 1;
        }
        if (x !=0 || y != 0) {
            releaseAllControls();
            //gVehicleNoDrive->setDriveTorque(0, distance * 1000.0f);
        //    gVehicleNoDrive->setDriveTorque(1, distance * 1000.0f);
            //gVehicleNoDrive->setDriveTorque(2, distance * 1000.0f);
            //gVehicleNoDrive->setDriveTorque(3, distance * 1000.0f);
            PxRigidBody *carBody = gVehicleNoDrive->getRigidDynamicActor();
            PxTransform globalTransform = carBody->getGlobalPose();
            PxVec3 vForce = globalTransform.q.rotate(PxVec3(y, 0, x));
            carBody->addForce(vForce * MOVEMENT_FORCE);

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

    //}
}
void PhysicsManager::rotatePlayer(int entityID, float x) {
    gVehicleNoDrive->getRigidDynamicActor()->setAngularVelocity(physx::PxVec3(0, -x * ANGULAR_MOMENTUM, 0));
}
void PhysicsManager::stopKey() {
    if (currentState != 2) {
        //releaseAllControls();
        //startAccelerateReverseMode();
        currentState = 2;
    }
}
void PhysicsManager::leftKey() {
    if (currentState != 3) {

        //releaseAllControls();
    //startTurnHardLeftMode();
    currentState = 3;
    }
}
void PhysicsManager::rightKey() {
    if (currentState != 4) {
        //releaseAllControls();
        //startTurnHardRightMode();
        currentState = 4;
    }
}
// This function is public. Probably intended as a sort of soft reset at the end of a match
//    that will set up Physics to be restarted as everything gets loaded in for a new match
// Also called from the Destructor before the Destructor performs any hard clean up.
void PhysicsManager::cleanupPhysics()
{
    if (hasStarted) { // basically this is just so that we don't try and destroy
        // the physics before it's been initalized
        PX_UNUSED(m_bInteractive);

        gVehicleNoDrive->getRigidDynamicActor()->release();
        gVehicleNoDrive->free();
        gGroundPlane->release();
        gBatchQuery->release();
        gVehicleSceneQueryData->free(gAllocator);
        gFrictionPairs->release();
        PxCloseVehicleSDK();


        gMaterial->release();
        gCook->release();
        gScene->release();
        gDispatcher->release();
        gPhysics->release();
        PxPvdTransport* transport = gPvd->getTransport();
        gPvd->release();
#ifdef _DEBUG
        transport->release();
#endif
        //manager->release();
        gFoundation->release();




        hasStarted = false;
        gScene = NULL; // change value back to null in case cleanup is called twice
    }
}

// Make sure these are well commented so others looking at this code can
//    see how this works. These will probably be called from PhysicsComponents exclusively
//    which will create an additional layer from Entities to Physics Manager.
//    You could make PhysicsComponent a Friend Class for Physics Manager and make these
//    functions private. This will ensure that only Physics Components can call these functions
//    which will prevent users from misusing your code or intended design, allowing for less
//    errors down the line due to misuse.

PxRigidDynamic *PhysicsManager::createCubeObject(float x,float y, float z, float size) {
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(size, size, size), *gMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidDynamic *body = gPhysics->createRigidDynamic(localTm);
    body->attachShape(*shape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    gScene->addActor(*body);
    return body;
    //physicsDynamicObject *sphere = new physicsDynamicObject();
    //dynamicObjects.push_back(sphere);
}
PxRigidDynamic *PhysicsManager::createCubeObjectPlayer(float x, float y, float z, float size) {
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.05f, 0.05f, 0.05f), *gMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidDynamic *body = gPhysics->createRigidDynamic(localTm);
    body->attachShape(*shape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    gScene->addActor(*body);
    return body;
    //physicsDynamicObject *sphere = new physicsDynamicObject();
    //dynamicObjects.push_back(sphere);
}
PxVehicleNoDrive *PhysicsManager::createPlayerEntity() {
    //Create a vehicle that will drive on the plane.
    snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc();
    gVehicleNoDrive = createVehicleNoDrive(vehicleDesc, gPhysics, gCook);
    PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
    gVehicleNoDrive->getRigidDynamicActor()->setGlobalPose(startTransform);
    gScene->addActor(*gVehicleNoDrive->getRigidDynamicActor());

    //Set the vehicle to rest in first gear.
    //Set the vehicle to use auto-gears.
    gVehicleNoDrive->setToRestState();

    gVehicleModeTimer = 0.0f;
    gVehicleOrderProgress = 0;

    return gVehicleNoDrive;
    //startBrakeMode();
    //physicsDynamicObject *sphere = new physicsDynamicObject();
    //dynamicObjects.push_back(sphere);
}

/********************************************************************************\
 * Private Functions                                                            *
\********************************************************************************/

// This should be a private function for Physics Manager as it uses it internally to
//    obfuscate the public interface of the Physics Manager
mat4 PhysicsManager::getMat4(PxTransform transform) {
    PxMat44 mat44 = PxMat44(transform);
    mat4 returnMatrix;

    // Convert the PxMat44 matrix to a glm::mat4
    memcpy(value_ptr(returnMatrix), &mat44, sizeof(mat4));

    // Return mat4 matrix
    return returnMatrix;
}

// I made this a private function for PhysicsManager. As an outsider using PhysicsManager
//    as a blackbox, I don't know what "createStack" means or why I want to call that function.
//    I assume PhysicsManager knows what it means and what it wants to use it for, therefore, I assume
//    this should be a private function.
void PhysicsManager::createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
    for (PxU32 i = 0; i < size; i++)
    {
        for (PxU32 j = 0; j < size - i; j++)
        {
            PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
            PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            gScene->addActor(*body);
        }
    }
    shape->release();
}

// Made this function private, externally this makes sense, but when and why it should
//    be called it up to the PhysicsManager. I added an update function that will be called
//    every frame, but you can decide how often to call this function and how it works under the hood.
void PhysicsManager::stepPhysics()
{
    hasStarted = true;
    PX_UNUSED(m_bInteractive
        /*Private Variable, lasts lifetime of PhysicsManager since initialization, may need to be redesigned as needed?*/);
    const PxF32 timestep = UPDATE_TIME_IN_SECONDS;

    //Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
    //incrementDrivingMode(timestep);

    //Raycasts.
    PxVehicleWheels* vehicles[1] = { gVehicleNoDrive };
    PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

    //Vehicle update.
    const PxVec3 grav = gScene->getGravity();
    PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicleNoDrive->mWheelsSimData.getNbWheels()} };
    PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

    gScene->simulate(timestep);
    gScene->fetchResults(true);
}
