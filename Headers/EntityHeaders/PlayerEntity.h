#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityComponentHeaders/PhysicsComponent.h"

class PlayerEntity :
	public Entity
{
public:
	PlayerEntity(int iID, const vec3* vPosition);
	virtual ~PlayerEntity();

	void initializePlayer(const string& sFileName, 
							const Material* pMaterial, 
							const string& sShaderType,
							float fScale);

private:
	Mesh* m_pMesh;
	RenderComponent* m_pRenderComponent;
	PhysicsComponent* m_pPhysicsComponent;
};

