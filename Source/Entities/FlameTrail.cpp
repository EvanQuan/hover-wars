#include "EntityHeaders/FlameTrail.h"
#include "EntityManager.h"
#include "SoundManager.h"
#include "EntityComponentHeaders/AnimationComponent.h"
#include "EntityHeaders/HovercraftEntity.h"

using namespace SpriteSheetDatabase;

// Default Constructor
FlameTrail::FlameTrail(int iID, int iOwnerID,
                       const vec3* vPosition,
                       const vec3* vColor,
                       float fHeight, float fWidth )
    : InteractableEntity( iID, iOwnerID, *vPosition, INTER_FLAME_TRAIL )
{
    m_fHeight           = fHeight;
    m_fWidth            = fWidth;
    m_sSpriteSheetInfo  = vSpriteInformation[eSpriteEnum::FIRE_SPRITE];
    m_pPhysXMngr        = PHYSICS_MANAGER;
    m_vColor            = *vColor;
}

// Destructor
FlameTrail::~FlameTrail()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Virtual Functions                                  *
\****************************************************************/

void FlameTrail::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    // copy and pasted from rocket, may be differenet
    if (m_iOwnerID != pOther->getID())
    {
        // Tell the Other Entity that they've been hit via the Inherited Collision Handler
        InteractableEntity::handleCollision(pOther, iColliderMsg, iVictimMsg);
    }
    else if (eEntityType::ENTITY_INTERACTABLE == pOther->getType() &&
        eInteractType::INTER_ROCKET == static_cast<InteractableEntity*>(pOther)->getInteractableType())
    {
        // Tell the rocket to explode.
        pOther->handleCollision(this, iVictimMsg, iColliderMsg);
    }
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
    m_pAnimationComponent->initializeComponentAsBillboard(m_pMesh, &m_sSpriteSheetInfo,
                                                          m_fHeight, m_fWidth);

    // Generate Physics Actor
    m_sName = to_string(m_iID) + " " + to_string(m_iOwnerID);

    // Generate the Render Component
    m_pRenderComponent = pEntityManager->generateRenderComponent(m_iID, m_pMesh, false,
        ShaderManager::eShaderType::BILLBOARD_SHDR, GL_POINTS);

    // Get a Physics Component for this Entity
    m_pPhysicsComponent = pEntityManager->generatePhysicsComponent(m_iID);
}

void FlameTrail::update(float fTimeInSeconds)
{
    // Local variables
    bool bDeletionFlag = false;

    // Update the Physics for the flame trail based on current Flame Trail
    for (vector<sReferenceBlock>::iterator pIter = m_pReferenceMap.begin();
        pIter != m_pReferenceMap.end();
        ++pIter)
    {
        // Track Duration
        pIter->fDuration -= fTimeInSeconds;

        // Handle Certain Thresholds of the Duration
        if (pIter->fDuration <= 0.0f)      // Delete the Physics Actor
        {
            m_pPhysXMngr->removeRigidDynamicObj(pIter->pActorRef);
            bDeletionFlag = true;
        }
    }

    if (bDeletionFlag)
    {
        m_pReferenceMap.erase(
            remove_if(
                m_pReferenceMap.begin(),
                m_pReferenceMap.end(),
                [](sReferenceBlock const& p) {return p.fDuration <= 0.0f; }
            ),
            m_pReferenceMap.end()
        );
    }
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
     unsigned int iFlameIndex = m_pAnimationComponent->addBillboard(vPosition, vNormal, &m_vColor);

    // Store the duration locally to manage the Physics Component.
    sReferenceBlock pNewBlock;
    pNewBlock.fDuration = m_sSpriteSheetInfo.fDuration;
    m_pPhysXMngr->createCylinderObject(m_sName.c_str(), vPosition, m_fHeight * 0.5f,
                                    m_fWidth * 0.5f, &pNewBlock.pActorRef);
    m_pReferenceMap.push_back(pNewBlock);

    // Grab Pointer to HashKey to give to Physics Component as Name.
    //m_pPhysicsComponent->initializeFlame(sHashKey.c_str(), vPosition, m_fHeight * 0.5f, m_fWidth * 0.5f);
}
