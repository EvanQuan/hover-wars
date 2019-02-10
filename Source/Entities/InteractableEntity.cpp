#include "EntityHeaders/InteractableEntity.h"
#include "EntityManager.h"

// Default Constructor
InteractableEntity::InteractableEntity(int iID, const vec3* vPosition)
	: Entity( iID, *vPosition )
{
	
}

// Destructor
InteractableEntity::~InteractableEntity()
{
	// Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions								*
\****************************************************************/

void InteractableEntity::update(float fTimeInMilliseconds)
{
	/* Not Implemented */
}

/****************************************************************\
 * Load Functions												*
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void InteractableEntity::loadAsBillboard(const vec3* vNormal, int iHeight, int iWidth, const Material* pMaterial)
{
	// Load Render Component and get Texture Dimensions
	int iTextureHeight(0), iTextureWidth(0);
	m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, true, ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);
	assert(nullptr != m_pRenderComponent);
	m_pRenderComponent->loadMaterial(pMaterial);
	m_pRenderComponent->getDiffuseTextureDimensions(&iTextureHeight, &iTextureWidth);
	vec2 vUVStart = vec2(0.0f);
	vec2 vUVEnd = vec2(	static_cast<float>(iWidth) / static_cast<float>(iTextureWidth),
						static_cast<float>(iHeight) / static_cast<float>(iTextureHeight));

	// Generate the Mesh
	m_pBillboardMesh = MESH_MANAGER->generateBillboardMesh(m_vPosition, *vNormal, vUVStart, vUVEnd, iHeight, iWidth );

	// Initialize Render Component with Mesh.
	m_pRenderComponent->initializeComponent(m_pBillboardMesh);

	// PHYSICSTODO: Set up Physics Component as a Physics Object for an interactable Object. This may be temporary, but maybe make it a cylinder?
//	m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
	//m_pPhysicsComponent->initializeComponent(true, m_pBillboardMesh); // PHYSICSTODO
}