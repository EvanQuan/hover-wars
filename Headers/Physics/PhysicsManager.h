#pragma once
//#include "characterkinematic/PxControllerManager.h"
//#include "PxFoundation.h"
#include "stdafx.h"


/***************************************************************
 * Name: PhysicsManager
 * Written by: James Cote and Austin Eaton
 * Description: Provides overlaying framework for PhysX implementation.
 *		Maintains PhysX updating, initialization and clean up for entire
 *		program. Initialization and Deconstruction happens in main and
 *		updates are called in Entity Manager. Callbacks for collisions should
 *		probably be set up in Entity Manager so that the Entity Manager can
 *		form a message and forward it onto the necessary Entities. Beyond that,
 *		Physics Manager shouldn't maintain a reference to Entity Manager, but
 *		Entity Manager should know about Physics Manager for running that aspect of
 *		the engine.
\************************************************************************************/
class PhysicsManager {
public:
	// Singleton Implementation - If necessary, initializes static instance and returns
	//			reference to Singleton Physics Manager.
	static PhysicsManager* getInstance();	
	virtual ~PhysicsManager();

	// Update function for Physics, technically called every frame, but
	//	internally can stall its update until a certain tick time.
	//	fTimeDelta is given in seconds and is usually a fraction of a second.
	//	<= 1/60th of a second to maintain 60FPS, may be <= 1/30th of a second in
	//	the case of multiplayer, depending on performance.
	void update(duration<float> fTimeDelta);
	void initPhysics(bool interactive);
	void cleanupPhysics( );		// This could serve different purposes depending on implementation.
								// Either it cleans up currently running physics so we can return to main
								// menu and set up a new physics environment or it deconstructs all the physics
								// for final clean up.  I suggest, having this be the former and calling this in the
								// the destructor before doing final clean up in the destructor.

	// Scene Setting Functions
	void createSphereObject();	// This is probably called by Physics Components as necessary to set themselves up within
								// the physics scene. Additional specific functions could be generated as neccessary.

private:
	// Singleton Implementation: make all possible constructors private and inaccessable
	//		to disallow multiple instance of the physics manager.
	static PhysicsManager *m_pInstance;
	PhysicsManager();	// Private Constructor for Singleton Implementation
	PhysicsManager(const PhysicsManager* pCopy); // Overloaded Private Copy Constructor for Singleton Implementation
	PhysicsManager* operator=(const PhysicsManager* pCopy);	// Overloaded assignment operator for Singleton Implementation

	// Private Variables
	bool m_bInteractive;			// Not sure what this is for, but maybe set it on initialization and maintain it over
									// the longevity of the Physics Manager? 
	float m_fTimeSinceLastUpdate;	// Increments every update to track how long since last Physics Update.
	// Moving these from Constructor to Here to privatize these variables used by the Physics
	//	manager
	PxDefaultAllocator gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation*			gFoundation = NULL;
	PxPhysics*				gPhysics = NULL;

	PxDefaultCpuDispatcher*	gDispatcher = NULL;
	PxScene*				gScene = NULL;
	PxControllerManager*	manager = NULL;

	PxMaterial*				gMaterial = NULL;

	PxPvd*                  gPvd = NULL;
	PxCooking *				gCook;
	PxReal stackZ = -3.0f;

	// Private Functions - Not necessary for outside classes to have access to these, they don't need to know about them.
	void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
	void stepPhysics(); // This probably functions within the update function to be used as necessary.
};


// If These are necessary internally for the Physics Manager, make these private structs that can be maintained
//	within the Physics Manager as needed. Look at EntityManager for using unique_ptrs to avoid potential memory
//	leaks of raw pointers. If they're internal though, you probably don't need to store them as pointers and could probably
//	maintain them on the stack in a vector. But if heap makes more sense, please use unique_ptrs.
//	Privatising these maintains an Object Oriented Design in protecting information within the class.
//class physicsDynamicObject {
//private: 
//	PxRigidDynamic* body;
//public:
//	physicsDynamicObject(float x, float y, float z, float cubeSize);
//	void addForce(float x, float y, float z);
//	glm::mat4 getTransformMatrix();
//};
//class physicsStaticObject {
//private:
//	PxRigidStatic* body;
//public:
//	physicsStaticObject(float x, float y, float z, float cubeSize);
//	glm::mat4 getTransformMatrix();
//};