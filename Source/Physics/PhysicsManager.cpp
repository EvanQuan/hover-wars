/************
 * Includes *
\************/
#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "Physics/PhysicsManager.h"
#include <iostream>
#include <vector>

/**************\
 * Namespaces *
\**************/
/*using namespace physx; Already declared in stdafx.h**/

/***********\
 * DEFINES *
\***********/
#define PVD_HOST "127.0.0.1"
#define UPDATE_TIME_IN_SECONDS 30.0f

/****************************************************************************\
 * Singleton Implementation													*
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

// Default Constructor for Physics Manager
//	Initializes all default settings for Physics Manager
PhysicsManager::PhysicsManager()
{
	// Not sure why these would be global as you had them.
	//	Make these private within the PhysicsManager header and maintain
	//	them throughout the lifespan of the Physics Manager.
	/*PxDefaultAllocator		*/gAllocator;
	/*PxDefaultErrorCallback	*/gErrorCallback;
	/*PxFoundation*				*/gFoundation = NULL;
	/*PxPhysics*				*/gPhysics = NULL;
	/*							*/
	/*PxDefaultCpuDispatcher*	*/gDispatcher = NULL;
	/*PxScene*					*/gScene = NULL;
	/*PxControllerManager*		*/manager = NULL;
	/*							*/
	/*PxMaterial*				*/gMaterial = NULL;
	/*							*/
	/*PxPvd*					*/gPvd = NULL;
	/*PxCooking *				*/gCook;
	/*PxReal					*/stackZ = -3.0f;
	/*Left these here to show change made by moving these to PhysicsManager Private Variables*/
}

// PhysicsManager Destructor -
//	Cleans up all Physics and destructs any initialized heap memory 
//	used by PhysicsManager
PhysicsManager::~PhysicsManager()
{
	// do soft clean up to ensure nothing is missed.
	cleanupPhysics();

	// PHYSICSTODO: finish any remaining final clean up here:
}

/**
 * Commenting out this functionality for now, does this need to be public? private?
 *	What is this from a blackbox standpoint if it's public? How does it serve a purpose for
 *	PhysicsManager if it's private? **
physicsDynamicObject::physicsDynamicObject(float x, float y, float z, float cubeSize) {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(cubeSize, cubeSize, cubeSize), *gMaterial);
	PxTransform localTm(PxVec3(x, y, z));
	body = gPhysics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
}//*/

/****************************************************************************\
 * Public Functions															*
\****************************************************************************/

// Name: Update
// Description: Provides Frame by Frame update functionality for Physics.
//		Updates on a different interval than standard framerate.
void PhysicsManager::update(duration<float> fTimeDelta)
{
	// Grab the timeDelta as a float instead of the templated duration type.
	float fDelta = static_cast<float>(fTimeDelta.count());

	// Increment Time since last update and check to see if an update is ready.
	//	if an update is ready, decrement the internal timer to reset it accurately
	//	then perform the update.
	if ((m_fTimeSinceLastUpdate += fDelta) >= UPDATE_TIME_IN_SECONDS)
	{
		// Don't reset to 0.0f as it's probably not at the Update Time Exactly.
		//	this preserves an accuracy w.r.t. the game timer.
		m_fTimeSinceLastUpdate -= UPDATE_TIME_IN_SECONDS;

		// PHYSICSTODO: Do Update Here
		stepPhysics(); // for example
	}
}

// Initialization of Physics will happen here, this will be called in main, immediately
//	after constructing the PhysicsManager Instance.
void PhysicsManager::initPhysics(bool interactive)
{
	// For my private variables, I append the 'm_' to the front to say, "This is my variable"
	//	"my" referring to the namespace of the function: PhysicsManager. I also put the small case
	//	character in front of the Variable name to signify the type of variable it is, 
	//	'b' for bool, 'i' for int, 'p' for pointer, etc.
	m_bInteractive = interactive;	// Storing this boolean internally to set up
									// physics to persistantly be this way. Maybe 
									// this needs to be designed differently?

	// Comment each of these lines, tell us what each function is doing and why it is necessary.
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
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
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial); //TODO add this to the create plane function in static entity
	gScene->addActor(*groundPlane);
	gCook = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
	if (!gCook)
		std::cout << ("PxCreateCooking failed!") << std::endl;

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
	for (PxU32 i = 0; i < 5; i++)
		createStack(PxTransform(PxVec3(0, 0, stackZ += 3.0f)), 5, 0.1f);

	//createDynamic(PxTransform(PxVec3(0, 4, 1)), PxSphereGeometry(1), PxVec3(0, -1, -1));
//createDynamic(PxTransform(PxVec3(0, 4, 0)), PxSphereGeometry(1), PxVec3(0, -1, 0));
}

// This function is public. Probably intended as a sort of soft reset at the end of a match
//	that will set up Physics to be restarted as everything gets loaded in for a new match
// Also called from the Destructor before the Destructor performs any hard clean up.
void PhysicsManager::cleanupPhysics()
{
	PX_UNUSED(m_bInteractive);
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();
	//manager->release();
	gFoundation->release();

}

// Make sure these are well commented so others looking at this code can
//	see how this works. These will probably be called from PhysicsComponents exclusively
//	which will create an additional layer from Entities to Physics Manager.
//	You could make PhysicsComponent a Friend Class for Physics Manager and make these
//	functions private. This will ensure that only Physics Components can call these functions
//	which will prevent users from misusing your code or intended design, allowing for less
//	errors down the line due to misuse.
void PhysicsManager::createSphereObject() {
	//physicsDynamicObject *sphere = new physicsDynamicObject();
	//dynamicObjects.push_back(sphere);
}


/********************************************************************************\
 * Private Functions															*
\********************************************************************************/

// This should be a private function for Physics Manager as it uses it internally to
//	obfuscate the public interface of the Physics Manager
mat4 PhysicsManager::getMat4(PxTransform transform) {
	float matrixArray[4][4];
	PxMat44 mat44 = PxMat44(transform);
	for (int i = 0; i < 4; i++) {
		matrixArray[0][i] = mat44.column0.x;
		matrixArray[1][i] = mat44.column0.y;
		matrixArray[2][i] = mat44.column0.z;
		matrixArray[3][i] = mat44.column0.w;
	}
	return glm::make_mat4x4(&matrixArray);
}


/** Saving this code for however you want to design it.
glm::mat4 physicsStaticObject::getTransformMatrix() {
	return getMat4(body->getGlobalPose());
}
glm::mat4 physicsDynamicObject::getTransformMatrix() {
	return getMat4(body->getGlobalPose());
}
void physicsDynamicObject::addForce(float x, float y, float z) {
	body->addForce(PxVec3(x, y, z));
}
physicsStaticObject::physicsStaticObject(float x, float y, float z, float cubeSize) {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(cubeSize, cubeSize, cubeSize), *gMaterial);
	PxTransform localTm(PxVec3(x, y, z));
	body = gPhysics->createRigidStatic(localTm);
	body->attachShape(*shape);
	gScene->addActor(*body);
}//*/

// I made this a private function for PhysicsManager. As an outsider using PhysicsManager
//	as a blackbox, I don't know what "createStack" means or why I want to call that function.
//	I assume PhysicsManager knows what it means and what it wants to use it for, therefore, I assume
//	this should be a private function.
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
//	be called it up to the PhysicsManager. I added an update function that will be called
//	every frame, but you can decide how often to call this function and how it works under the hood.
void PhysicsManager::stepPhysics()
{
	PX_UNUSED(m_bInteractive
		/*Private Variable, lasts lifetime of PhysicsManager since initialization, may need to be redesigned as needed?*/);
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
}
