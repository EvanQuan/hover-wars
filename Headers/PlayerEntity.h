#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "EntityComponentHeaders/RenderComponent.h"

class PlayerEntity :
	public Entity
{
public:
	PlayerEntity(int iID, vec3 vPosition);
	virtual ~PlayerEntity();

	void initializePlayer(const string& sFileName, 
							const string& sTextureLocation, 
							const string& sShaderType);

private:
	Mesh* m_pMesh;
	RenderComponent* m_pRenderComponent;
};

