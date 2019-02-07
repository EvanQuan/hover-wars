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
									const string& sShaderType)
{
	// Load Mesh and Rendering Component
	m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, m_vPosition);
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, false, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);

	// Ensure that the Entity Manager returned a Render Component and Initialize it.
	assert(nullptr != m_pRenderComponent);
	m_pRenderComponent->initializeComponent(m_pMesh, pMaterial);
}
