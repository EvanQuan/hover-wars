/************
 * Includes *
\************/
#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "Physics/PhysicsCallBack.h"
#include "Physics/PhysicsManager.h"
#include "vehicle/PxVehicleUtil.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include <iostream>
#include <vector>
#include <sstream>

/***********\
 * DEFINES *
\***********/
#define PVD_HOST "127.0.0.1"
#define UPDATE_TIME_IN_SECONDS (1.0f / 60.0f) // Physics should update every 1/60th of a second regardless of the game's framerate.

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

snippetvehicle::VehicleDesc PhysicsManager::initVehicleDesc(PxVec3 chassisDims)
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const PxF32 chassisMass = 300.0f;
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
        m_fTimeSinceLastUpdate = 0.0f;

        // Step Physics at 1/60th of a second.
        stepPhysics(UPDATE_TIME_IN_SECONDS); 
    }
}
static PxFilterFlags filterShader(
    PxFilterObjectAttributes attributes0,
    PxFilterData filterData0,
    PxFilterObjectAttributes attributes1,
    PxFilterData filterData1,
    PxPairFlags& pairFlags,
    const void* constantBlock,
    PxU32 constantBlockSize)
{
    pairFlags = PxPairFlag::eSOLVE_CONTACT;
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
    pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
    pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
    return PxFilterFlags();
}
// Initialization of Physics will happen here, this will be called in main, immediately
//    after constructing the PhysicsManager Instance.
void PhysicsManager::initPhysics(bool interactive)
{
    if (gScene != NULL) {
        return;
    }
    
    m_bInteractive = interactive;

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
    sceneDesc.filterShader = filterShader;
    cb = new PhysicsCallBack();
    sceneDesc.simulationEventCallback = cb;
    sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_PAIRS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;
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
    PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
    PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    gVehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
    gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = snippetvehicle::createFrictionPairs(gMaterial);

    //Create a plane to drive on.
    PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);

    PxRigidStatic* gGroundPlane = snippetvehicle::createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
    gScene->addActor(*gGroundPlane);
    gGroundPlane->setName("Cube");
    staticObjects.push_back(gGroundPlane);
    





    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //TEST CODE
    //createSphereObject(0,0,0,3);
    //createMeshObject(3,3,3,5,"memeteam.txt");
    //createPlayerEntity(0,0,0);
}

// This function is public. Probably intended as a sort of soft reset at the end of a match
//    that will set up Physics to be restarted as everything gets loaded in for a new match
// Also called from the Destructor before the Destructor performs any hard clean up.
void PhysicsManager::cleanupPhysics()
{
    if (hasStarted) { // basically this is just so that we don't try and destroy
        // the physics before it's been initalized
        PX_UNUSED(m_bInteractive);
        for (PxVehicleNoDrive *vehicle : vehicles) {
            vehicle->getRigidDynamicActor()->release();
            vehicle->free();
        }
        for (PxRigidStatic *object : staticObjects) {
            object->release();
        }
        gBatchQuery->release();
        gVehicleSceneQueryData->free(gAllocator);
        gFrictionPairs->release();
        PxCloseVehicleSDK();
        delete cb;

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
PxRigidStatic *PhysicsManager::createMeshObject(float x, float y, float z,float scale,string filename) {
    PxShape* shape = gPhysics->createShape(PxTriangleMeshGeometry(generateMesh(filename,scale)), *gMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidStatic *body = gPhysics->createRigidStatic(localTm);
    body->attachShape(*shape);
    gScene->addActor(*body);
    staticObjects.push_back(body);
    body->setName("mesh");
    return body;
}
const int MAXLINE = 256;
PxTriangleMesh *PhysicsManager::generateMesh(string filename,float m_scale) {
    ifstream inFile(filename, ios::in);
    vector<PxVec3> vertices;
    vector<PxU32> verticesFinal;
    while (inFile)
    {
        string currLine;
        getline(inFile, currLine);
        if (currLine[0] == 'v' && currLine[1] == ' ') {
            currLine = currLine.substr(2, currLine.length() - 2);
            stringstream ss(currLine);
            PxVec3 vertex;
            ss >> vertex.x;
            ss >> vertex.y;
            ss >> vertex.z;
            vertex *= m_scale;
            cout << vertex.x << "," << vertex.y << "," << vertex.z << "\n";
            vertices.push_back(vertex);
        }
        else if (currLine[0] == 'f' && currLine[1] == ' ') {
            currLine = currLine.substr(2, currLine.length() - 2);
            stringstream ss;
            PxU32 indice;
            int i;
            for (i = 0; currLine[i] != ' ' && currLine[i] != '/'&& currLine[i] != '\n'; i++) {
                ss << currLine[i];
            }
            ss >> indice;
            std::cout << indice << ",";
            stringstream ss2;
            while (currLine[i] != ' ')
                i++;
            i++;
            for (; currLine[i] != ' ' && currLine[i] != '/'&& currLine[i] != '\n'; i++) {
                ss2 << currLine[i];
            }
            PxU32 indice2;
            ss2 >> indice2;
            std::cout << indice2 << ",";
            stringstream ss3;
            while (currLine[i] != ' ')
                i++;
            i++;
            for (; currLine[i] != ' ' && currLine[i] != '/'&& currLine[i] != '\n'; i++) {
                ss3 << currLine[i];
            }
            PxU32 indice3;

            ss3 >> indice3;
            std::cout << indice3 << "\n";
            verticesFinal.push_back(indice3);
            verticesFinal.push_back(indice2);
            verticesFinal.push_back(indice);

        }

    }
    PxTolerancesScale scale;
    PxCookingParams params(scale);
    // disable mesh cleaning - perform mesh validation on development configurations
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    // disable edge precompute, edges are set for each triangle, slows contact generation
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
    // lower hierarchy for internal mesh
    gCook->setParams(params);
    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = vertices.size();
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = &vertices[0];

    meshDesc.triangles.count = verticesFinal.size()/3;
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data = &verticesFinal[0];

    PxDefaultMemoryOutputStream writeBuffer;
    bool status = gCook->cookTriangleMesh(meshDesc, writeBuffer);
    if (!status){
        std::cout << "cooking failed" << std::endl;
        return NULL;
    }
    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    return gPhysics->createTriangleMesh(readBuffer);
}
PxRigidStatic *PhysicsManager::createCubeObject(float x,float y, float z, float sizeX,float sizeY,float sizeZ) {
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(sizeX, sizeY, sizeZ), *gMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidStatic *body = gPhysics->createRigidStatic(localTm);
    body->attachShape(*shape);
    gScene->addActor(*body);
    staticObjects.push_back(body);
    body->setName("Cube");
    return body;
}
PxRigidStatic *PhysicsManager::createSphereObject(float x, float y, float z, float radius) {
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidStatic *body = gPhysics->createRigidStatic(localTm);
    body->attachShape(*shape);
    gScene->addActor(*body);
    staticObjects.push_back(body);
    body->setName("Sphere");
    return body;
}
PxVehicleNoDrive *PhysicsManager::createPlayerEntity(float x, float y, float z, float sizeX, float sizeY, float sizeZ) {
    //Create a vehicle that will drive on the plane.
    snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc(PxVec3(sizeX, sizeY, sizeZ));
    gVehicleNoDrive = createVehicleNoDrive(vehicleDesc, gPhysics, gCook);
    PxTransform startTransform(PxVec3(x, y+(vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), z), PxQuat(PxIdentity));
    gVehicleNoDrive->getRigidDynamicActor()->setGlobalPose(startTransform);
    gScene->addActor(*gVehicleNoDrive->getRigidDynamicActor());
    //Set the vehicle to rest in first gear.
    gVehicleNoDrive->setToRestState();
    gVehicleModeTimer = 0.0f;
    gVehicleOrderProgress = 0;
    vehicles.push_back(gVehicleNoDrive);
    gVehicleNoDrive->getRigidDynamicActor()->setName("vehicle");

    return gVehicleNoDrive;
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

// Made this function private, externally this makes sense, but when and why it should
//    be called it up to the PhysicsManager. I added an update function that will be called
//    every frame, but you can decide how often to call this function and how it works under the hood.
void PhysicsManager::stepPhysics(float fTimeDelta)
{
    hasStarted = true;
    PX_UNUSED(m_bInteractive
        /*Private Variable, lasts lifetime of PhysicsManager since initialization, may need to be redesigned as needed?*/);
    const PxF32 timestep = fTimeDelta;

    //Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
    //incrementDrivingMode(timestep);
    gScene->simulate(timestep);
    gScene->fetchResults(true);
}
bool PhysicsManager::updateCar(PxVehicleNoDrive *vehicle, float fTimeDelta) {
    const PxF32 timestep = fTimeDelta;
    //Raycasts.
    PxVehicleWheels* vehicles[1] = { vehicle };
    PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

    //Vehicle update.
    const PxVec3 grav = gScene->getGravity();
    PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, vehicle->mWheelsSimData.getNbWheels()} };
    PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);
    return wheelQueryResults[0].isInAir && wheelQueryResults[1].isInAir && wheelQueryResults[2].isInAir && wheelQueryResults[3].isInAir;
}
