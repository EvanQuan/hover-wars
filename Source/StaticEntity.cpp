#include "StaticEntity.h"
#include "EntityManager.h"

// Default Constructor
StaticEntity::StaticEntity(int iID, vec3 vPosition)
	: Entity( iID, vPosition )
{
	
}

StaticEntity::StaticEntity(const StaticEntity& pCopy)
	: Entity( pCopy )
{
	m_pMesh = pCopy.m_pMesh;
	m_pRenderComponent = pCopy.m_pRenderComponent;
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
	m_pMesh = MESH_MANAGER->generatePlaneMesh(iHeight, iWidth, m_vPosition, vNormal);
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, true, ShaderManager::eShaderType::PLANE_SHDR, GL_TRIANGLE_STRIP);

	assert(nullptr != m_pRenderComponent);
	m_pRenderComponent->initializeComponent(m_pMesh->getVertices(), m_pMesh->getNormals(), m_pMesh->getUVs());
}

void StaticEntity::loadAsSphere(float fRadius)
{

}

void StaticEntity::loadFromFile(const string& sFileName)
{

}