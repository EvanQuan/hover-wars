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
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void InteractableEntity::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

/****************************************************************\
 * Load Functions                                                *
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void InteractableEntity::loadAsBillboard(const vec3* vNormal, float fHeight, float fWidth, const Material* pMaterial)
{
    // Load Render Component and get Texture Dimensions
    vec2 vUVStart = vec2(0.035f, 0.035f);
    vec2 vUVEnd = vec2(0.09f, 0.125f);

    Material pBillboardMaterial;
    pBillboardMaterial.fShininess = 0.0f;
    pBillboardMaterial.sDiffuseMap = "textures/Smoke30Frames.png";

    // Generate the Mesh
    m_pBillboardMesh = MESH_MANAGER->generateBillboardMesh(&pBillboardMaterial, this);
    m_pAnimationComponent = ENTITY_MANAGER->generateAnimationComponent(m_iID);
    m_pAnimationComponent->initializeComponentAsBillboard(m_pBillboardMesh, vec2(0.16666667f, 0.2f), vec2(0.0f), 6, 5, fHeight, fWidth);
    m_pAnimationComponent->addBillboard(&m_vPosition, vNormal);

    // Generate the Render Component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pBillboardMesh, true, ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);

    // PHYSICSTODO: Set up Physics Component as a Physics Object for an interactable Object. This may be temporary, but maybe make it a cylinder?
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pBillboardMesh); // PHYSICSTODO
}
