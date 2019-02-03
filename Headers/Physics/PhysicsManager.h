#pragma once
#include "characterkinematic/PxControllerManager.h"
#include "PxPhysicsAPI.h"
#include "PxFoundation.h"
using namespace physx;

void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);

void initPhysics(bool interactive);
void cleanupPhysics(bool interactive);
void stepPhysics(bool interactive);
class physicsDynamicObject {
private: 
	PxRigidDynamic* body;
public:
	physicsDynamicObject(float x, float y, float z, float cubeSize);
	void addForce(float x, float y, float z);
};
class physicsStaticObject {
private:
	PxRigidStatic* body;
public:
	physicsStaticObject(float x, float y, float z, float cubeSize);
};