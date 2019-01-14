#include "StaticEntity.h"
#include "EntityManager.h"

// Default Constructor
StaticEntity::StaticEntity(int iID, vec3 vPosition)
	: Entity( iID, vPosition )
{
	
}

// Destructor
StaticEntity::~StaticEntity()
{
	// Nothing to destruct
}

/****************************************************************\
 * Load Functions												*
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void StaticEntity::loadAsPlane(vec3 vNormal, int iHeight, int iWidth)
{
	m_pMesh = MESH_MANAGER->generatePlaneMesh( true, iHeight, iWidth, m_vPosition, vNormal);
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, true, ShaderManager::eShaderType::PLANE_SHDR, GL_TRIANGLE_STRIP);

	assert(nullptr != m_pRenderComponent);
	m_pRenderComponent->initializeComponent( m_pMesh );
}

// Load a Sphere with a given Radius
void StaticEntity::loadAsSphere(float fRadius)
{
	m_pMesh = MESH_MANAGER->generateSphereMesh(true, fRadius, m_vPosition);
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, true, ShaderManager::eShaderType::PLANE_SHDR, GL_TRIANGLE_STRIP);

	assert(nullptr != m_pRenderComponent);
	m_pRenderComponent->initializeComponent(m_pMesh);
}

// Load a Static Mesh from a given file
void StaticEntity::loadFromFile(const string& sFileName)
{
	// Grab the Mesh Object
	m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, m_vPosition, true);

	// Set up Render component
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, true, ShaderManager::eShaderType::PLANE_SHDR, GL_TRIANGLES);

	// Given that the component was generated successfully, initialize it.
	assert(nullptr != m_pRenderComponent);
	m_pRenderComponent->initializeComponent(m_pMesh);
}