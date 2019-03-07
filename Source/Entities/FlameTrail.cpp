#include "EntityHeaders/FlameTrail.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
FlameTrail::FlameTrail(int iID, int iOwnerID, const vec3* vPosition)
    : InteractableEntity( iID, iOwnerID, vPosition, FLAME_TRAIL_ENTITY )
{
    // aLl interactable entities need collision detection
    // m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
}

// Destructor
FlameTrail::~FlameTrail()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void FlameTrail::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void FlameTrail::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void FlameTrail::handleCollision(const Entity* pOther) const
void FlameTrail::handleCollision(Entity* pOther)
{
    /* Not Implemented */
}

/****************************************************************\
 * Billboard Functions                                          *
\****************************************************************/

// Load a Plane with a given Normal, Height and Width
void FlameTrail::loadAsBillboard(float fHeight, float fWidth)
{
    m_pObjectInfo.sObjMaterial.fShininess = 0.0f;
    m_pObjectInfo.sObjMaterial.sDiffuseMap = vSpriteInformation[eSpriteEnum::FIRE_SPRITE].sSheetLocation;
    
    vec3 vTempPos = vec3(5.0f);
    vec3 vTempNormal = vec3(0.0f, 1.0f, 0.0f);
    // Generate the Mesh
    m_pBillboardMesh = MESH_MANAGER->generateBillboardMesh(&m_pObjectInfo, this);
    m_pAnimationComponent = ENTITY_MANAGER->generateAnimationComponent(m_iID);
    m_pAnimationComponent->initializeComponentAsBillboard(m_pBillboardMesh, &vSpriteInformation[eSpriteEnum::FIRE_SPRITE], fHeight, fWidth);
    // TEST Billboard    m_pAnimationComponent->addBillboard(&vTempPos, &vTempNormal);

    // Generate the Render Component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pBillboardMesh, true, ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);

    // PHYSICSTODO: Set up Physics Component as a Physics Object for an interactable Object. This may be temporary, but maybe make it a cylinder?
    // m_pPhysicsComponent->initializeComponent(true, m_pBillboardMesh, &m_pObjectInfo.sObjBoundingBox, m_vPosition); // PHYSICSTODO

}

// Adds a new Billboard at a given position.
//  @vNormal: The Normal for the Billboard to rotate around.
//  @vPosition: The Position of the Center-Middle of the Billboard.
void FlameTrail::addBillboard(const vec3* vNormal, const vec3* vPosition)
{
    // Add a flame
    // TODO hard code name as player 1 flame
    const char* flameName = TYPE_TRAIL OWNER_PLAYER_1;
    m_vDimensions = vec3(1.0);
    m_pObjectInfo.vPosition = *vPosition; // Update the location
    m_pAnimationComponent->addBillboard(vPosition, vNormal);

    // This crashes the game
    // ENTITY_MANAGER->generateStaticCube(flameName, &m_pObjectInfo, &m_vDimensions);
}

/****************************************************************\
 * Powerup Functions                                          *
\****************************************************************/

void FlameTrail::loadAsPowerup(vec3* dimensions)
{
}
