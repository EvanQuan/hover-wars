#include "EntityHeaders/FlameTrail.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
FlameTrail::FlameTrail(int iID, int iOwnerID, const vec3* vPosition,
                            float fHeight, float fWidth )
    : InteractableEntity( iID, iOwnerID, *vPosition, FLAME_TRAIL_ENTITY )
{
    m_fHeight = fHeight;
    m_fWidth = fWidth;
}

// Destructor
FlameTrail::~FlameTrail()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

void FlameTrail::initialize()
{
    // Local Variables
    ObjectInfo pFlameTrailInfo;
    pFlameTrailInfo.sObjMaterial.sDiffuseMap = vSpriteInformation[eSpriteEnum::FIRE_SPRITE].sSheetLocation;

    // Generate the Mesh
    m_pMesh                 = MESH_MANAGER->generateBillboardMesh(&pFlameTrailInfo, this);
    m_pAnimationComponent   = ENTITY_MANAGER->generateAnimationComponent(m_iID);
    m_pAnimationComponent->initializeComponentAsBillboard(m_pMesh, &vSpriteInformation[eSpriteEnum::FIRE_SPRITE], m_fHeight, m_fWidth);

    // Generate the Render Component
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, false, ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);

    // Set up Physics Component for a flame trail
    // m_pPhysicsComponent->initializeComponent(true, m_pMesh, &m_pObjectInfo.sObjBoundingBox, m_vPosition); // PHYSICSTODO

}

void FlameTrail::update(float fTimeInMilliseconds)
{
    // Update the Physics for the flame trail based on current Flame Trail
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

// Adds a new Billboard at a given position.
//  @vNormal: The Normal for the Billboard to rotate around.
//  @vPosition: The Position of the Center-Middle of the Billboard.
void FlameTrail::spawnFlame(const vec3* vNormal, const vec3* vPosition)
{
    // Add a flame
    m_pAnimationComponent->addBillboard(vPosition, vNormal);
}
