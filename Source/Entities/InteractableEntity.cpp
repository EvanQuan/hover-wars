#include "EntityHeaders/InteractableEntity.h"
#include "EntityHeaders/HovercraftEntity.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
InteractableEntity::InteractableEntity(int iID, int iOwnerID, const vec3& vPosition, eInteractType eIEType)
    : Entity( iID, vPosition, ENTITY_INTERACTABLE )
{
    m_iOwnerID          = iOwnerID;  // Set the Owner ID for the Interactable Entity
    m_eInteractableType = eIEType;
}

// Destructor
InteractableEntity::~InteractableEntity()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

void InteractableEntity::initialize(const string& sFileName,
                                    const ObjectInfo* pObjectProperties,
                                    const string& sShaderType,
                                    float fScale)
{
    // Load Mesh and Rendering Component
    EntityManager* pEntityMngr = ENTITY_MANAGER;
    m_pMesh             = MESH_MANAGER->loadMeshFromFile(sFileName, pObjectProperties, m_sName, fScale);
    m_pRenderComponent  = pEntityMngr->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);
    m_pPhysicsComponent = pEntityMngr->generatePhysicsComponent(m_iID);
}

void InteractableEntity::update(float fTimeInSeconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. 
void InteractableEntity::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

/*
    Default functionality for Interactable Entities.
    Default: tells the other entity that they've been hit.

    @param pOther           entity that this entity collided with.
    @param iColliderMsg
    @param iVictimMsg
*/
void InteractableEntity::handleCollision(Entity* pOther,
                                         unsigned int iColliderMsg,
                                         unsigned int iVictimMsg)
{
    if (ENTITY_HOVERCRAFT == pOther->getType() && m_iOwnerID != pOther->getID())
    {
        eHovercraft eOwner = GAME_STATS->getEHovercraft(m_iOwnerID);
        HovercraftEntity* owner = ENTITY_MANAGER->getHovercraft(eOwner);
        HovercraftEntity* hit = static_cast<HovercraftEntity*>(pOther);

        // cout << owner << "[ID: " << m_iOwnerID << "] hit "
        // << GAME_STATS->getEHovercraft(hit->getID())
        // << "[ID: "<< hit->getID() << "]"<< endl;

        hit->getHitBy(eOwner, getAbility());
    }
}


