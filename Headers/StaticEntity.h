#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "MeshManager.h"
#include "EntityComponentHeaders/RenderComponent.h"

// Name: StaticEntity
// Written by: James Cote
// Description: General Entity for static objects in the game world.
class StaticEntity
	: public Entity
{
public:
	StaticEntity(int iID, vec3 vPosition);

	virtual ~StaticEntity();

	void loadAsPlane(vec3 vNormal, int iHeight, int iWidth);
	void loadAsSphere(float fRadius);
	void loadFromFile(const string& sFileName);

private:
	// Private Copy Constructor and Assignment Operator
	StaticEntity(const StaticEntity& pCopy);
	StaticEntity& operator=(const StaticEntity& pCopy);

	Mesh* m_pMesh; // Contains Vertex information about the mesh.
	RenderComponent* m_pRenderComponent; // Component for handling Rendering of the entity
};