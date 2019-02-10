#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"

// Name: InteractableEntity
// Written by: James Cote
// Description: General Entity for objects that players can interact with.
//		These types of entities can be objects such as pickups or killzones (i.e. flame paths).
//		Idea: these entities, when a player collides with them, will send a message to that player 
//			telling them what happens as a result of this collision. Different messages can be generated
//			by inheriting from this class and defining their own message.
class InteractableEntity
	: public Entity
{
public:
	InteractableEntity(int iID, const vec3* vPosition);
	virtual ~InteractableEntity();

	// Implementation of inherited functionality
	void update(float fTimeInMilliseconds);

	void loadAsBillboard(const vec3* vNormal, int iHeight, int iWidth, const Material* pMaterial);

private:
	// Private Copy Constructor and Assignment Operator
	InteractableEntity(const InteractableEntity& pCopy);
	InteractableEntity& operator=(const InteractableEntity& pCopy);

	GLuint m_iVertexArray;

	Mesh* m_pBillboardMesh;
	RenderComponent* m_pRenderComponent; // Component for handling Rendering of the entity
	PhysicsComponent* m_pPhysicsComponent; // Component for handling Physics for the Entity
};