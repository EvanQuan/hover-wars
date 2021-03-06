#pragma once
#include "characterkinematic/PxControllerManager.h"
#include "DataStructures/ObjectInfo.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "stdafx.h"

#include "vehicle/PxVehicleUtil.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include <vector>

// Forward Declaration
class Mesh;

#define WHEEL_COUNT 4
/***************************************************************
 * Name: PhysicsManager
 * Written by: James Cote, Austin Eaton, Evan Quan
 * Description: Provides overlaying framework for PhysX implementation.
 *        Maintains PhysX updating, initialization and clean up for entire
 *        program. Initialization and Deconstruction happens in main and
 *        updates are called in Entity Manager. Callbacks for collisions should
 *        probably be set up in Entity Manager so that the Entity Manager can
 *        form a message and forward it onto the necessary Entities. Beyond
 *        that, Physics Manager shouldn't maintain a reference to Entity
 *        Manager, but Entity Manager should know about Physics Manager for
 *        running that aspect of the engine.
\************************************************************************************/
class PhysicsManager {
public:
    // Singleton Implementation - If necessary, initializes static instance and returns
    //            reference to Singleton Physics Manager.
    static PhysicsManager* getInstance();    
    virtual ~PhysicsManager();

    // Update function for Physics, technically called every frame, but
    //    internally can stall its update until a certain tick time.
    //    fTimeDelta is given in seconds and is usually a fraction of a second.
    //    <= 1/60th of a second to maintain 60FPS, may be <= 1/30th of a second in
    //    the case of multiplayer, depending on performance.
    void update(float fTimeDelta);
    void initPhysics(bool interactive);
    void cleanupPhysics( );        // This could serve different purposes depending on implementation.
                                // Either it cleans up currently running physics so we can return to main
                                // menu and set up a new physics environment or it deconstructs all the physics
                                // for final clean up.  I suggest, having this be the former and calling this in the
                                // the destructor before doing final clean up in the destructor.

    // Scene Setting Functions
    //void createSphereObject();    // This is probably called by Physics Components as necessary to set themselves up within
                                // the physics scene. Additional specific functions could be generated as neccessary.
    physx::PxRigidStatic *createStaticMeshObject(const char* sEntityID, const Mesh* pMesh, const vec3* vPos);
    physx::PxRigidStatic *createCubeObject(const char* sEntityID, float x, float y, float z, float sizeX, float sizeY, float sizeZ);
    physx::PxVehicleNoDrive *createHovercraftEntity(const char* sEntityID, float x, float y, float z, float sizeX, float sizeY, float sizeZ);
    physx::PxRigidStatic *createSphereObject(const char* sEntityID, float x, float y, float z, float radius);
    void createRocketObjects(const char* cName, const mat4* m4Transform, const vec3 *vVelocity, float fBBLength, PxRigidDynamic** pReturnBody);
    void createCylinderObject(const char* cName, const vec3* vPosition, float fHeight, float fRadius, PxRigidDynamic** pReturnBody);
    void removeRigidActor(PxRigidActor* pActor);
    glm::mat4 getMat4(physx::PxTransform transform); // Internal Function to swap a PhysX Mat44 to a glm mat4 (column to row-major order)
    void stepPhysics(float fTimeDelta); // This probably functions within the update function to be used as necessary.
    bool updateCar(PxVehicleNoDrive *vehicle, float fTimeDelta);
    glm::vec3 getClosestNormalOnHeightMap(glm::vec3 pos);
    float getClosestNormalOnHeightMapDotProduct(glm::vec3 pos,glm::vec3 normDir);
    void createGroundPlane(float tileX, float tileY, int numTiles, float *heightmap,const char * name);
    int timesStepped = 0;
    PxRigidStatic* createFloorHeightMap(PxReal hfScale, PxU32 hfSize,PxReal heightScale, float *values);
    PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxRigidStatic *stuff);
private:
    glm::vec3 PhysicsManager::getNormVector(int x, int y);
    vector<vector<float>> heightMapVec;
    int tileNum;
    float tileDistance;
    // extra stuff to delete
    glm::vec3 getPointAt(int x, int y);
    std::vector<physx::PxTriangleMesh*> triangleMeshes;
    std::vector<physx::PxShape*> shapes;

    PxSimulationEventCallback *cb;
    std::vector<physx::PxVehicleNoDrive *> vehicles;
    std::vector<physx::PxRigidStatic *> staticObjects;
    std::vector<physx::PxRigidDynamic *> dynamicObjects;

    physx::PxTriangleMesh *generateMesh(string filename,float scale);
    int currentState = 0;
    snippetvehicle::VehicleSceneQueryData*      gVehicleSceneQueryData = NULL;
    physx::PxBatchQuery*                        gBatchQuery            = NULL;

    physx::PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

    physx::PxVehicleNoDrive*        gVehicleNoDrive       = NULL;

    physx::PxF32                    gVehicleModeLifetime  = 4.0f;
    physx::PxF32                    gVehicleModeTimer     = 0.0f;
    bool                            gVehicleOrderComplete = false;
    physx::PxU32                    gVehicleOrderProgress = 0;

    snippetvehicle::VehicleDesc initVehicleDesc(PxVec3 chassisDims);

    // Singleton Implementation: make all possible constructors private and inaccessable
    //        to disallow multiple instance of the physics manager.
    static PhysicsManager *m_pInstance;
    PhysicsManager();                                           // Private Constructor for Singleton Implementation
    PhysicsManager(const PhysicsManager* pCopy);                // Overloaded Private Copy Constructor for Singleton Implementation
    PhysicsManager* operator=(const PhysicsManager* pCopy);     // Overloaded assignment operator for Singleton Implementation

    // Private Variables
    bool m_bInteractive;            // Not sure what this is for, but maybe set it on initialization and maintain it over
                                    // the longevity of the Physics Manager? 
    float m_fTimeSinceLastUpdate;   // Increments every update to track how long since last Physics Update.
    // Moving these from Constructor to Here to privatize these variables used by the Physics
    //    manager
    physx::PxDefaultAllocator*      gAllocator;
    physx::PxDefaultErrorCallback*  gErrorCallback;
    physx::PxFoundation*            gFoundation     = NULL;
    physx::PxPhysics*               gPhysics        = NULL;

    physx::PxDefaultCpuDispatcher*  gDispatcher     = NULL;
    physx::PxScene*                 gScene          = NULL;

    physx::PxMaterial*              gCarMaterial    = NULL;
    physx::PxMaterial*              gWorldMaterial  = NULL;

    physx::PxPvd*                   gPvd            = NULL;
    physx::PxCooking *              gCook;

};
