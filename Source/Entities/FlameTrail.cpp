#include "EntityHeaders/FlameTrail.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
FlameTrail::FlameTrail(int iID, int iOwnerID, const vec3* vPosition,
                       float fHeight, float fWidth )
    : InteractableEntity( iID, iOwnerID, *vPosition, INTER_FLAME_TRAIL )
{
    m_fHeight = fHeight;
    m_fWidth = fWidth;
    m_sSpriteSheetInfo = vSpriteInformation[eSpriteEnum::FIRE_SPRITE];
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
    EntityManager* pEntityManager = ENTITY_MANAGER;
    pFlameTrailInfo.sObjMaterial.sDiffuseMap = m_sSpriteSheetInfo.sSheetLocation;

    // Generate the Mesh
    m_pMesh = MESH_MANAGER->generateBillboardMesh(&pFlameTrailInfo, this);
    m_pAnimationComponent = pEntityManager->generateAnimationComponent(m_iID);
    m_pAnimationComponent->initializeComponentAsBillboard(m_pMesh, &m_sSpriteSheetInfo, m_fHeight, m_fWidth);

    // Generate the Render Component
    m_pRenderComponent = pEntityManager->generateRenderComponent(m_iID, m_pMesh, false, ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);

    // Get a Physics Component for this Entity
    m_pPhysicsComponent = pEntityManager->generatePhysicsComponent(m_iID);
}

void FlameTrail::update(float fTimeInSeconds)
{
    // Local Variables
    bool bFlagForDeletion = false;

    // Update the Physics for the flame trail based on current Flame Trail
    for (unordered_map<string, float>::iterator pIter = m_pReferenceMap.begin();
        pIter != m_pReferenceMap.end();
        ++pIter)
    {
        // Track Duration
        pIter->second -= fTimeInSeconds;

        // Handle Certain Thresholds of the Duration
        if (pIter->second <= 0.0f)      // Delete the Physics Actor
        {
            m_pPhysicsComponent->removeInstance(pIter->first);
            bFlagForDeletion = true;
        }
        else if (pIter->second <= 1.0f) // Scale the Physics Actor
            m_pPhysicsComponent->scaleInstance(pIter->first, pIter->second);
    }

    // Clean up any References that are subject for deletion.
    if (bFlagForDeletion)
        m_pReferenceMap.erase(
            remove_if(
                m_pReferenceMap.begin(),
                m_pReferenceMap.end(),
                [](float & p) { return p <= 0.f; }
            ),
            m_pReferenceMap.end());
}

/****************************************************************\
 * Billboard Functions                                          *
\****************************************************************/

// Adds a new Billboard at a given position.
//  @vNormal: The Normal for the Billboard to rotate around.
//  @vPosition: The Position of the Center-Middle of the Billboard.
void FlameTrail::spawnFlame(const vec3* vNormal, const vec3* vPosition)
{
    // Add a flame To the Animation Component (Also adds it to the Mesh).
    unsigned int iFlameIndex = m_pAnimationComponent->addBillboard(vPosition, vNormal);

    // Store the duration locally to manage the Physics Component.
    string sHashKey = to_string(m_iID) + " " + to_string(iFlameIndex);
    m_pReferenceMap.insert(make_pair(sHashKey, m_sSpriteSheetInfo.fDuration));

    // Grab Pointer to HashKey to give to Physics Component as Name.
    unordered_map<string, float>::iterator pIter = m_pReferenceMap.find(sHashKey);
    m_pPhysicsComponent->initializeFlame(pIter->first.c_str(), vPosition, m_fHeight, m_fWidth);
}
