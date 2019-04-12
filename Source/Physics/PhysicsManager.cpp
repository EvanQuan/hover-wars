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
#include "Mesh.h"
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
#define CAR_STATIC_FRICTION 0.00f // 0.35f
#define WORLD_STATIC_FRICTION 0.00f // 0.35f
/*
Coefficient of dynamic friction

This determines the resistance to force when the car is already moving.
We want this to be high enough to prevent infinite drifting, and so players can
stop and change directions in a reasonably responsive manner.
We want this to be low enough that there some amount of drifting can be done to
make it feel like a hovercraft.
*/
#define CAR_DYNAMIC_FRICTION 1.00f // 0.35f
#define WORLD_DYNAMIC_FRICTION 1.00f // 0.35f
/*
Car Restitution

Affects how elastic collisions are. The greater this value, the more colliding
objects bounce away.

This should be relatively high to make car collisions satisfying.
*/
#define CAR_RESTITUTION 1.0f // 0.2f

// Definition for for Cap Radius of Rockets
#define CAP_RADIUS 0.5f // 0.2f is safe, 0.5 is large

/*
World Restitution

This should be low to make the world not bouncy.
*/
#define WORLD_RESTITUTION 0.1f

/*
Acceleration of gravity downwards

This affects how "floaty" objects feel when they are in the air.
Real world value is -9.81
Typically in games, this value is much greater than the real world value.

Acceleration : m/s^2

*/
#define GRAVITY -50.0f
/*
This affects the momentum of the vehicle.
The greater it is, the slow the car will take to accelerate, either from
neutral or in changing directions. To increase player input responsiveness,
decrease this.
The greater this value, the less responsive cars will be to forces such as
explosions or collisions.

Mass : kilograms
*/
#define CHASSIS_MASS 2000.0 // 2000

// the following are offsets related to the chasis center of mass position
#define XOFFSET_MULTIPLYER 0
#define YOFFSET_MULTIPLYER -3.0f // -3.0f // Original -1.5f
#define ZOFFSET_MULTIPLYER 0

#define XOFFSET_ADDITIVE 0      // 0
#define YOFFSET_ADDITIVE 0.65f  // 0.65f
#define ZOFFSET_ADDITIVE 0.25f  // 0.25f

// this is multiplied to direction of the car in order to offset the rocket creation
#define NORAMLIZED_DISTANCE_ROCKET_MULTIPLIER 3

// 90 Degrees as Radians
#define RADIANS_90  1.5708f

/****************************************************************************\
 * Singleton Implementation                                                 *
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
    // manager = NULL;
    gCarMaterial = NULL;
    gWorldMaterial = NULL;
    
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
    const PxF32 chassisMass = CHASSIS_MASS;
    const PxVec3 chassisMOI
    ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
        (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
        (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
    //  The rogin is at center of the chassis mesh
    // Set the center of mas to be below this point and a little towards the front
    // x - side
    // y - height
    // z - front
    float offsetX = chassisDims.x*XOFFSET_MULTIPLYER + XOFFSET_ADDITIVE;
    float offsetY = chassisDims.y*YOFFSET_MULTIPLYER + YOFFSET_ADDITIVE;
    float offsetZ = chassisDims.z*ZOFFSET_MULTIPLYER + ZOFFSET_ADDITIVE;
    // cout << offsetX << " " << offsetY << " " << offsetZ << endl;
    const PxVec3 chassisCMOffset=PxVec3(offsetX, offsetY, offsetZ);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass = 20.0f;
    const PxF32 wheelRadius = 0.5f;
    const PxF32 wheelWidth = 0.4f;
    const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
    const PxU32 nbWheels = WHEEL_COUNT;

    snippetvehicle::VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = gCarMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = gCarMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
}

/****************************************************************************\
 * Public Functions                                                         *
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
PxRigidStatic* PhysicsManager::createFloorHeightMap(PxReal hfScale, PxU32 hfSize, PxReal heightScale, float *values)
{

    PxU32 hfNumVerts = hfSize * hfSize;
    PxHeightFieldSample* samples = new PxHeightFieldSample[hfNumVerts];
    memset(samples, 0, hfNumVerts * sizeof(PxHeightFieldSample));
    int i = 0;
    heightMapVec.clear();
    for (PxU32 x = 0; x < hfSize; x++) {
        vector<float> xValues;
        for (PxU32 y = 0; y < hfSize; y++)
        {
            i++;
            samples[x + y * hfSize].height = (PxI16)(values[x + y * hfSize] / heightScale);
            xValues.push_back(values[x + y * hfSize]);
            cout << "values in height :" << values[x + y * hfSize] << "," << x << "," << y << endl;
            samples[x + y * hfSize].setTessFlag();
            samples[x + y * hfSize].materialIndex0 = 1;
            samples[x + y * hfSize].materialIndex1 = 1;
        }
        heightMapVec.push_back(xValues);
    }

    cout << "times ran: " << i << endl;

    PxHeightFieldDesc hfDesc;
    hfDesc.format = PxHeightFieldFormat::eS16_TM;
    hfDesc.nbColumns = hfSize;
    hfDesc.nbRows = hfSize;
    hfDesc.samples.data = samples;
    hfDesc.samples.stride = sizeof(PxHeightFieldSample);

    PxHeightField* heightField = gCook->createHeightField(hfDesc, gPhysics->getPhysicsInsertionCallback());

    PxTransform pose = PxTransform(PxIdentity);
    pose.p = PxVec3(-((hfSize) / 2.0f * hfScale), 0, -((hfSize ) / 2.0f * hfScale));

    PxRigidStatic* hfActor = gPhysics->createRigidStatic(pose);
    if (!hfActor)
        cout << "creating heightfield actor failed" << endl;

    PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, hfScale, hfScale);
    PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *gWorldMaterial);
    //setCCDActive(*hfShape);
    if (!hfShape)
        cout << "creating heightfield shape failed" << endl;
    hfActor->setName(C_SUBTYPE_GROUND + "");// :p fix later @austin
    gScene->addActor(*hfActor);

    return hfActor;
}
//
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
PxRigidStatic* PhysicsManager::createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxRigidStatic * body)
{


    //Get the plane shape so we can set query and simulation filter data.
    PxShape* shapes[1];
    body->getShapes(shapes, 1);

    //Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
    PxFilterData qryFilterData;
    snippetvehicle::setupDrivableSurface(qryFilterData);
    shapes[0]->setQueryFilterData(qryFilterData);

    //Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
    shapes[0]->setSimulationFilterData(simFilterData);

    return body;
}
void PhysicsManager::initPhysics(bool interactive)
{
    cout << "initPhysics" << endl;
    if (gScene != NULL) {
        cout << "\tgScene !=NULL, not initing..." << endl;
        return;
    }
    cout << "\tstart..." << endl;
    
    m_bInteractive = interactive;

    gAllocator = new PxDefaultAllocator();
    gErrorCallback = new PxDefaultErrorCallback();
    // Comment each of these lines, tell us what each function is doing and why it is necessary.
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *gAllocator, *gErrorCallback);
    
#ifdef _DEBUG
    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
#else
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);
#endif
   
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
#ifdef _DEBUG
    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    bool extensionsInitialized = PxInitExtensions(*gPhysics, gPvd);
    if (!extensionsInitialized) {
        std::cout << ("PxInitExtensions failed!") << std::endl;
    }
#endif
    gCarMaterial = gPhysics->createMaterial(CAR_STATIC_FRICTION, CAR_DYNAMIC_FRICTION, CAR_RESTITUTION);
    gWorldMaterial = gPhysics->createMaterial(WORLD_STATIC_FRICTION, WORLD_DYNAMIC_FRICTION, WORLD_RESTITUTION);

    cout << "gWorldmaterial initialized" << endl;

    gCook = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
    if (!gCook) {
        std::cout << ("PxCreateCooking failed!") << std::endl;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////


    bool resultVehicle = PxInitVehicleSDK(*gPhysics); // @AustinEaton : boolean initialized but not referenced
    PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
    PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    gVehicleSceneQueryData = snippetvehicle
                             ::VehicleSceneQueryData
                             ::allocate(1, PX_MAX_NB_WHEELS, 1, 1,
                                        snippetvehicle::WheelSceneQueryPreFilterBlocking,
                                        NULL, *gAllocator);
    gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = snippetvehicle::createFrictionPairs(gCarMaterial);

    //Create a plane to drive on.


    //Add a plane to the scene.
    //PxShape* shape = gPhysics->createShape(PxBoxGeometry(200, 1, 200), *gWorldMaterial);
    //PxTransform localTm(PxVec3(0, -1, 0));
    //PxRigidStatic *body = gPhysics->createRigidStatic(localTm);
    //body->attachShape(*shape);
    



    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //TEST CODE
    //createSphereObject(0,0,0,3);
    //createMeshObject(3,3,3,5,"memeteam.txt");
    //createHovercraftEntity(0,0,0);
    //createRocketP(5,5,5,1,0,1);
}

// This function is public. Probably intended as a sort of soft reset at the end of a match
//    that will set up Physics to be restarted as everything gets loaded in for a new match
// Also called from the Destructor before the Destructor performs any hard clean up.
void PhysicsManager::cleanupPhysics()
{
    if (hasStarted) { // basically this is just so that we don't try and destroy
        // the physics before it's been initalized
        PX_UNUSED(m_bInteractive);

        triangleMeshes.clear();
        for (PxShape *shape : shapes) {
            shape->release();
        }
        shapes.clear();
        for (PxVehicleNoDrive *vehicle : vehicles) {
            vehicle->getRigidDynamicActor()->release();
            vehicle->free();
        }
        vehicles.clear();

        for (PxRigidStatic *object : staticObjects) {
            object->release();
        }
        staticObjects.clear();

        dynamicObjects.clear();

        for (PxTriangleMesh *object : triangleMeshes) {
            object->release();
        }
        triangleMeshes.clear();

        gBatchQuery->release();
        gVehicleSceneQueryData->free(*gAllocator);
        gFrictionPairs->release();
        PxCloseVehicleSDK();
        delete cb;

        gCarMaterial->release();
        gWorldMaterial->release();

        gCook->release();
        gScene->release();
        gDispatcher->release();
        gPhysics->release();
#ifdef _DEBUG
        PxPvdTransport* transport = gPvd->getTransport();
        gPvd->disconnect();
        gPvd->release();
        transport->release();

        PxCloseExtensions();
#endif
        
        gFoundation->release();


        delete gAllocator;
        delete gErrorCallback;

        hasStarted = false;
        gScene = NULL; // change value back to null in case cleanup is called twice

        // basic variable init
        gFoundation = NULL;
        gPhysics = NULL;
        gDispatcher = NULL;
        gScene = NULL;
        gCarMaterial = NULL;
        gWorldMaterial = NULL;
        
        gPvd = NULL;
    }
}
void PhysicsManager::createGroundPlane(float tileX, float tileY, int numTiles,float *heightmap, const char* name) {

    PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);

    tileNum = numTiles;
    tileDistance = tileX;
    PxRigidStatic *body = createFloorHeightMap(tileX, numTiles, 0.1f, heightmap);

    PxRigidStatic* gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gWorldMaterial, gPhysics, body);
    gGroundPlane->setName(name);
    // cout << "\"" << gGroundPlane->getName() << "\"" << endl;
    gScene->addActor(*gGroundPlane);
    staticObjects.push_back(gGroundPlane);
}
// Make sure these are well commented so others looking at this code can
//    see how this works. These will probably be called from PhysicsComponents exclusively
//    which will create an additional layer from Entities to Physics Manager.
//    You could make PhysicsComponent a Friend Class for Physics Manager and make these
//    functions private. This will ensure that only Physics Components can call these functions
//    which will prevent users from misusing your code or intended design, allowing for less
//    errors down the line due to misuse.
PxRigidStatic *PhysicsManager::createStaticMeshObject(const char* sEntityID, const Mesh* pMesh, const vec3* vPos) {

    // Local Variables
    PxConvexMesh* pPxConMesh = nullptr;
    PxCookingParams params = gCook->getParams();
    PxConvexMeshDesc pPxConMeshDesc;
    PxDefaultMemoryOutputStream writeBuffer;
    PxRigidStatic *pPxRigidBody = nullptr;
    PxShape* pPxShape;
    const PxU32 numTrisPerLeaf = 15;

    // Use BVH34 midphase
    params.midphaseDesc = PxMeshMidPhase::eBVH34;

    // Setup cooking parameters
    // disable mesh cleaning - perform mesh validation on development configurations
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    // disable edge precompute, edges are set for each triangle, slows contact generation
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
    // lower hierarchy for internal mesh
    params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = numTrisPerLeaf;
    gCook->setParams(params);

    // Set up Mesh Description from Given Mesh
    pPxConMeshDesc.points.count = pMesh->getVertices().size();
    pPxConMeshDesc.points.stride = sizeof(PxVec3);
    pPxConMeshDesc.points.data = pMesh->getVertices().data();
    pPxConMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

//#ifdef _DEBUG
//    PX_ASSERT(gCook->validateTriangleMesh(meshDesc));
//#endif

    // Cook the Mesh
    if (!gCook->cookConvexMesh(pPxConMeshDesc, writeBuffer))
        std::cout << "cooking failed" << std::endl;
    else
    {
        PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        pPxConMesh = gPhysics->createConvexMesh(readBuffer);
        pPxShape = gPhysics->createShape(PxConvexMeshGeometry(pPxConMesh), *gWorldMaterial);
        PxVec3 pPxPos;
        memcpy(&pPxPos, vPos, sizeof(vec3));
        PxTransform localTm(pPxPos);
        pPxRigidBody = gPhysics->createRigidStatic(localTm);
        pPxRigidBody->attachShape(*pPxShape);
        pPxRigidBody->setName(sEntityID);
        gScene->addActor(*pPxRigidBody);

        staticObjects.push_back(pPxRigidBody);
    }

    return pPxRigidBody;
}

PxRigidStatic *PhysicsManager::createCubeObject(const char* sEntityID, float x,float y, float z, float sizeX,float sizeY,float sizeZ) {
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(sizeY, sizeX, sizeZ), *gWorldMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidStatic *body = gPhysics->createRigidStatic(localTm);
    body->attachShape(*shape);
    body->setName(sEntityID);
    // cout << "\"" << body->getName() << "\"" << endl;
    gScene->addActor(*body);
    staticObjects.push_back(body);

    shapes.push_back(shape);
    return body;
}
PxRigidStatic *PhysicsManager::createSphereObject(const char* sEntityID, float x, float y, float z, float radius) {
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gWorldMaterial);
    PxTransform localTm(PxVec3(x, y, z));
    PxRigidStatic *body = gPhysics->createRigidStatic(localTm);
    body->attachShape(*shape);
    body->setName(sEntityID);
    // cout << "\"" << body->getName() << "\"" << endl;
    gScene->addActor(*body);
    staticObjects.push_back(body);
    shapes.push_back(shape);
    return body;
}

// Name: createRocketObjects
// Written by: James Coté & Austin Eaton
// Description: Generates a rocket object and launches it in the scene.
void PhysicsManager::createRocketObjects(const char* cName, const mat4* m4Transform, const vec3 *vVelocity, float fBBLength, PxRigidDynamic** pReturnBody)
{
    // Generate Shape for the Rocket.
    PxShape* pShape = gPhysics->createShape(PxCapsuleGeometry(CAP_RADIUS, fBBLength), *gWorldMaterial);

    // Generate Transform to given position.
    PxMat44 pxTransform;
    memcpy(&pxTransform, m4Transform, sizeof(mat4));
    PxTransform pxLocalTransform(pxTransform);
    pShape->setLocalPose(PxTransform(PxQuat(RADIANS_90, PxVec3(0.0f, 1.0f, 0.0f))));

    // Set Velocity
    PxVec3 pxRocketVel;
    memcpy(&pxRocketVel, vVelocity, sizeof(vec3));

    // Set up Physics Body
    *pReturnBody = gPhysics->createRigidDynamic(pxLocalTransform);
    (*pReturnBody)->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    (*pReturnBody)->setMaxAngularVelocity(0.0f);
    (*pReturnBody)->setLinearVelocity(pxRocketVel);
    (*pReturnBody)->attachShape(*pShape);
    (*pReturnBody)->setName(cName);

    // Add To Scene
    // cout << "\"" << (*pReturnBody)->getName() << "\"" << endl;
    gScene->addActor(*(*pReturnBody));
}

// Name: createCylinderObject
// Written By: James Coté
// Description: Generates a Cylindrical bounding box at a given position with a
// specified Height and Radius. Object ignores gravity. Used for creating flame
// trail entities.
void PhysicsManager::createCylinderObject(const char* cName, const vec3* vPosition,
                                       float fHeight, float fRadius, PxRigidDynamic** pReturnBody)
{
    // Generate Cylindrical Shape
    PxShape* pShape = gPhysics->createShape(PxCapsuleGeometry(fRadius, fHeight), *gWorldMaterial);

    // Generate Transform to given position.
    PxVec3 pxFlamePos;
    PxQuat pShapeRotation = PxQuat(RADIANS_90, PxVec3(0.0f, 0.0f, 1.0f));
    memcpy(&pxFlamePos, vPosition, sizeof(vec3));
    pShape->setLocalPose(PxTransform(pShapeRotation));
    PxTransform pxLocalTransform(pxFlamePos);

    // Set up Physics Body
    *pReturnBody = gPhysics->createRigidDynamic(pxLocalTransform);
    (*pReturnBody)->setMass(0.01f);
    (*pReturnBody)->setRigidDynamicLockFlags(   PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
                                                PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
                                                PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
                                                PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
                                                PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
                                                PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);
    (*pReturnBody)->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    (*pReturnBody)->attachShape(*pShape);
    (*pReturnBody)->setName(cName);

    // Add to Scene
    // cout << "\"" << (*pReturnBody)->getName() << "\"" << endl;
    gScene->addActor(*(*pReturnBody));
}

// Removes a Rigid Dynamic Object from the scene and releases the actor
void PhysicsManager::removeRigidActor(PxRigidActor* pActor)
{
    gScene->removeActor(*pActor, false);
    pActor->release();
}


PxVehicleNoDrive *PhysicsManager::createHovercraftEntity(const char* sEntityID, float x, float y, float z, float sizeX, float sizeY, float sizeZ) {
    //Create a vehicle that will drive on the plane.
    snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc(PxVec3(sizeX, sizeY, sizeZ));
    gVehicleNoDrive = createVehicleNoDrive(vehicleDesc, gPhysics, gCook);
    PxTransform startTransform(PxVec3(x, y+(vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), z), PxQuat(PxIdentity));

    PxRigidDynamic* actor = gVehicleNoDrive->getRigidDynamicActor();
    actor->setGlobalPose(startTransform);
    actor->setName(sEntityID); // TODO this was just added?, doesn't seem to be the source of the problem
    // cout << "Player: \"" << actor->getName() << "\"" << endl;
    gScene->addActor(*actor);
    //Set the vehicle to rest in first gear.
    gVehicleNoDrive->setToRestState();
    gVehicleModeTimer = 0.0f;
    gVehicleOrderProgress = 0;
    vehicles.push_back(gVehicleNoDrive);
    // gVehicleNoDrive->getRigidDynamicActor()->setName(sEntityID);

    dynamicObjects.push_back(actor);


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
    timesStepped++;
    hasStarted = true;
    PX_UNUSED(m_bInteractive
        /*Private Variable, lasts lifetime of PhysicsManager since initialization, may need to be redesigned as needed?*/);
    const PxF32 timestep = fTimeDelta;

    //Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
    //incrementDrivingMode(timestep);
    gScene->simulate(timestep);
    gScene->fetchResults(true);
    //Suspension sweeps (instead of raycasts).
}

/*
    Update the car over a period of time.
*/
glm::vec3 PhysicsManager::getPointAt(int x, int y) {
    if (x < 0 ) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    if (x > tileNum-1) {
        x = tileNum-1;
    }
    if (y > tileNum-1) {
        y = tileNum-1;
    }
    return vec3(static_cast<float>(x - (tileNum >> 1)) * tileDistance,
                heightMapVec.at(y).at(x),
                static_cast<float>(y - (tileNum >> 1)) * tileDistance);
}
glm::vec3 PhysicsManager::getNormVector(int x, int y) {
    vec3 diff1 = getPointAt(x + 1, y) - getPointAt(x, y);
    vec3 diff2 = getPointAt(x, y + 1) - getPointAt(x, y);
    //std::cout <<"diff1:" << diff1.x << "," << diff1.y << "," << diff1.z << endl;
    //std::cout <<"diff2:"<< diff2.x << "," << diff2.y << "," << diff2.z << endl;
    vec3 A = cross(diff1, diff2);
    vec3 B = cross(getPointAt(x - 1, y) - getPointAt(x, y), getPointAt(x, y - 1) - getPointAt(x, y));
    return normalize(A + B);

}
float PhysicsManager::getClosestNormalOnHeightMapDotProduct(glm::vec3 pos,glm::vec3 norm) {
    float fTileMid = (static_cast<float>(tileNum) * 0.5f);
    float fX = (pos.x / tileDistance) + fTileMid;
    float fZ = (pos.z / tileDistance) + fTileMid;
    int XindexFloor =   static_cast<int>(floor(fX));
    int XindexCeil =    static_cast<int>(ceil(fX));
    int YindexFloor =   static_cast<int>(floor(fZ));
    int YindexCeil =    static_cast<int>(ceil(fZ));
    vec3 normalizeFloorXY = getNormVector(XindexFloor,YindexFloor);
    vec3 normalizeCeilXY = getNormVector(XindexCeil, YindexCeil);
    vec3 normalizeFloorXCeilY = getNormVector(XindexFloor, YindexCeil);
    vec3 normalizeCeilXFloorY = getNormVector(XindexCeil, YindexFloor);
    return std::min(
        std::min(
            abs(dot(normalizeCeilXY,norm)),
            abs(dot(normalizeFloorXY, norm))   ),
        std::min(
            abs(dot(normalizeFloorXCeilY, norm)),
            abs(dot(normalizeCeilXFloorY, norm)))); // hey bro i heard you like functions so i put a function in a function in a function in a function so that this would function.
}
glm::vec3 PhysicsManager::getClosestNormalOnHeightMap(glm::vec3 pos) {
    float fTileMid = (static_cast<float>(tileNum) * 0.5f);
    int Xindex = static_cast<int>((pos.x / tileDistance) + fTileMid);
    int Yindex = static_cast<int>((pos.z / tileDistance) + fTileMid);
    vec3 vCurrPoint = getPointAt(Xindex, Yindex);
    vec3 diff1 = getPointAt(Xindex + 1, Yindex) - vCurrPoint;
    vec3 diff2 = getPointAt(Xindex, Yindex + 1) - vCurrPoint;
    //std::cout <<"diff1:" << diff1.x << "," << diff1.y << "," << diff1.z << endl;
    //std::cout <<"diff2:"<< diff2.x << "," << diff2.y << "," << diff2.z << endl;
    vec3 A = cross(diff2, diff1);
    vec3 B = cross(getPointAt(Xindex, Yindex - 1) - vCurrPoint, getPointAt(Xindex - 1, Yindex) - vCurrPoint);
    return normalize(A + B);
}
bool PhysicsManager::updateCar(PxVehicleNoDrive *vehicle, float fTimeDelta) {
    const PxF32 timestep = fTimeDelta;
    //Raycasts.
    //Sweeps provide more information about the geometry under the wheel.
    PxVehicleWheels* vehicleWheels[1] = { vehicle };
    PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicleWheels, raycastResultsSize, raycastResults);
    //Vehicle update.
    const PxVec3 grav = gScene->getGravity();
    PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults,vehicle->mWheelsSimData.getNbWheels()} };
    PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicleWheels, vehicleQueryResults);
//    cout << "1: " << wheelQueryResults[0].isInAir
//        << " 2: " << wheelQueryResults[1].isInAir
//        << " 3: " << wheelQueryResults[2].isInAir
//        << " 4: " << wheelQueryResults[3].isInAir << endl;
    // return wheelQueryResults[0].isInAir
        // && wheelQueryResults[1].isInAir
        // && wheelQueryResults[2].isInAir
        // && wheelQueryResults[3].isInAir;
    return raycastResults->getNbAnyHits() == 1 ? true : false;
}

// bool PhysicsManager::updateRocket(PxRigidDynamic *rocket, float fTimeDelta) {

// }
