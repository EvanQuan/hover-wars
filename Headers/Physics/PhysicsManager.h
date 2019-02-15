#pragma once
#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "stdafx.h"

#include "vehicle/PxVehicleUtil.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"

/***************************************************************
 * Name: PhysicsManager
 * Written by: James Cote and Austin Eaton
 * Description: Provides overlaying framework for PhysX implementation.
 *        Maintains PhysX updating, initialization and clean up for entire
 *        program. Initialization and Deconstruction happens in main and
 *        updates are called in Entity Manager. Callbacks for collisions should
 *        probably be set up in Entity Manager so that the Entity Manager can
 *        form a message and forward it onto the necessary Entities. Beyond that,
 *        Physics Manager shouldn't maintain a reference to Entity Manager, but
 *        Entity Manager should know about Physics Manager for running that aspect of
 *        the engine.
\************************************************************************************/
class PhysicsManager {
public:
    // Singleton Implementation - If necessary, initializes static instance and returns
    //            reference to Singleton Physics Manager.
    static PhysicsManager* getInstance();    
    virtual ~PhysicsManager();

    void forwardKey();
    void stopKey();
    void leftKey();
    void rightKey();
    void handleControllerInputMove(float x, float y);
    void handleControllerInputRotate(float x, float y);

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
    physx::PxRigidDynamic *createCubeObject(float x, float y, float z, float size);
    physx::PxRigidDynamic *createCubeObjectPlayer(float x, float y, float z, float size);
    physx::PxVehicleNoDrive *createPlayerEntity();
    glm::mat4 getMat4(physx::PxTransform transform); // Internal Function to swap a PhysX Mat44 to a glm mat4 (column to row-major order)
    void stepPhysics(); // This probably functions within the update function to be used as necessary.

private:
    int currentState = 0;
    snippetvehicle::VehicleSceneQueryData*      gVehicleSceneQueryData = NULL;
    physx::PxBatchQuery*                        gBatchQuery            = NULL;

    physx::PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

    physx::PxVehicleNoDrive*        gVehicleNoDrive       = NULL;

    physx::PxF32                    gVehicleModeLifetime  = 4.0f;
    physx::PxF32                    gVehicleModeTimer     = 0.0f;
    bool                            gVehicleOrderComplete = false;
    physx::PxU32                    gVehicleOrderProgress = 0;

    snippetvehicle::VehicleDesc initVehicleDesc();

    void releaseAllControls();
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
    physx::PxDefaultAllocator       gAllocator;
    physx::PxDefaultErrorCallback   gErrorCallback;
    physx::PxFoundation*            gFoundation     = NULL;
    physx::PxPhysics*               gPhysics        = NULL;
    physx::PxRigidStatic*           gGroundPlane    = NULL;

    physx::PxDefaultCpuDispatcher*  gDispatcher     = NULL;
    physx::PxScene*                 gScene          = NULL;
    physx::PxControllerManager*     manager         = NULL;

    physx::PxMaterial*              gMaterial       = NULL;

    physx::PxPvd*                   gPvd            = NULL;
    physx::PxCooking *              gCook;
    physx::PxReal stackZ = -3.0f;

    // Private Functions - Not necessary for outside classes to have access to these, they don't need to know about them.
    void createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);
};
