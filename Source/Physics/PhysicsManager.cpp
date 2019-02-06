#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "Physics/PhysicsManager.h"
#include <iostream>
#include <vector>


using namespace physx;

#define PVD_HOST "127.0.0.1"

PxDefaultAllocator		gAllocator;
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
physicsDynamicObject::physicsDynamicObject(float x, float y, float z, float cubeSize) {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(cubeSize, cubeSize, cubeSize), *gMaterial);
	PxTransform localTm(PxVec3(x, y, z));
	body = gPhysics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
}
glm::mat4 getMat4(PxTransform transform) {
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
}
PhysicsManager * PhysicsManager::instance = nullptr;

void PhysicsManager::initPhysics(bool interactive)
{
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
void PhysicsManager::stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
}

void PhysicsManager::cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();
	//manager->release();
	gFoundation->release();

}
PhysicsManager *PhysicsManager::getInstance() {
	if (instance == nullptr) {
		instance = new PhysicsManager();
	}
	return instance;
}
void PhysicsManager::createSphereObject() {
	physicsDynamicObject *sphere = new physicsDynamicObject();
	dynamicObjects.push_back(sphere);
}

