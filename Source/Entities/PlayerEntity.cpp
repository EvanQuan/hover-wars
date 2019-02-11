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

/****************************************************************\
 * Inherited Pure Virtual Functions								*
\****************************************************************/

void PlayerEntity::update(float fTimeInMilliseconds)
{
	// New Transformation Matrix
	mat4 m4NewTransform = mat4(1.0f);

	// Get the Transformation from the Physics component
	m_pPhysicsComponent->getTransformMatrix(&m4NewTransform);

	// If there's a new Transformation, apply it to the Mesh.
	if (mat4(1.0f) != m4NewTransform)
		m_pMesh->addInstance(&m4NewTransform);	// TODO: Mesh doesn't remember the scale it had, this will reset the scale since the physics won't know it. 
												//		I'll have to change this to make sure the Mesh remembers its scale.
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

	// PHYSICSTODO: Set up Physics Component as a Dynamic Physics Object for a player
	m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
	m_pPhysicsComponent->initializeComponent(true, m_pMesh);
}
