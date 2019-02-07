#pragma once
#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
#include "stdafx.h"
using namespace physx;

class PhysicsManager {
	vector<physicsDynamicObject> dynamicObjects;
	static PhysicsManager *instance;
	void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
	void initPhysics(bool interactive);
	void cleanupPhysics(bool interactive);
	void stepPhysics(bool interactive);
	void createSphereObject();
	PhysicsManager *getInstance();
};

class physicsDynamicObject {
private: 
	PxRigidDynamic* body;
public:
	physicsDynamicObject(float x, float y, float z, float cubeSize);
	void addForce(float x, float y, float z);
	glm::mat4 getTransformMatrix();
};
class physicsStaticObject {
private:
	PxRigidStatic* body;
public:
	physicsStaticObject(float x, float y, float z, float cubeSize);
	glm::mat4 getTransformMatrix();
};