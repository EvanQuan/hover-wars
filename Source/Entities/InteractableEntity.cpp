#include "EntityHeaders/InteractableEntity.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
InteractableEntity::InteractableEntity(int iID, const vec3* vPosition)
    : Entity( iID, *vPosition, INTERACTABLE_ENTITY )
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
 * Billboard Functions                                          *
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void InteractableEntity::loadAsBillboard(float fHeight, float fWidth)
{
    ObjectInfo sBillboardObjectInfo;
    sBillboardObjectInfo.sObjMaterial.fShininess = 0.0f;
    sBillboardObjectInfo.sObjMaterial.sDiffuseMap = vSpriteInformation[eSpriteEnum::FIRE_SPRITE].sSheetLocation;

    vec3 vTempPos = vec3(5.0f);
    vec3 vTempNormal = vec3(0.0f, 1.0f, 0.0f);
    // Generate the Mesh
    m_pBillboardMesh = MESH_MANAGER->generateBillboardMesh(&sBillboardObjectInfo, this);
    m_pAnimationComponent = ENTITY_MANAGER->generateAnimationComponent(m_iID);
    m_pAnimationComponent->initializeComponentAsBillboard(m_pBillboardMesh, &vSpriteInformation[eSpriteEnum::FIRE_SPRITE], fHeight, fWidth);
    m_pAnimationComponent->addBillboard(&vTempPos, &vTempNormal);

    // Generate the Render Component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pBillboardMesh, true, ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);

    // PHYSICSTODO: Set up Physics Component as a Physics Object for an interactable Object. This may be temporary, but maybe make it a cylinder?
    //m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID); // PHYSICSTODO: The parameters for this could be modified as you see fit.
    //m_pPhysicsComponent->initializeComponent(true, m_pBillboardMesh); // PHYSICSTODO
}

// Adds a new Billboard at a given position.
//  @vNormal: The Normal for the Billboard to rotate around.
//  @vPosition: The Position of the Center-Middle of the Billboard.
void InteractableEntity::addBillboard(const vec3* vNormal, const vec3* vPosition)
{
    m_pAnimationComponent->addBillboard(vPosition, vNormal);
}
