#include "EntityHeaders/PlayerEntity.h"
#include "MeshManager.h"
#include "EntityManager.h"

PlayerEntity::PlayerEntity(int iID, const vec3* vPosition)
	: Entity(iID, *vPosition)
{

}

PlayerEntity::~PlayerEntity()
{

}

// Initializes Player Entity information
void PlayerEntity::initializePlayer(const string& sFileName,
									const Material* pMaterial,
									const string& sShaderType,
									float fScale)
{
	// Load Mesh and Rendering Component
	m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pMaterial, fScale, m_vPosition);
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, false, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);
}
