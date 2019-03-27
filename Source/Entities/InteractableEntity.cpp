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

// Name: handleCollision
// Written by: James Cote
// Description: Default functionality for Interactable Entities.
//      Default: tells the other entity that they've been hit.
void InteractableEntity::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    // NOTE: Before  it used to look like this:
    //  static_cast<HovercraftEntity*>(pOther)->getHitBy(static_cast<eHovercraft>(m_iOwnerID));

    // The problem is that m_iOwnerID is the entity ID of the owner, which
    // is a number that only has meaning to Entity identificaiton and
    // management. This ID does not relate to the owner's eHovercraft
    // value, which is what the game stats cares about.

    // As a result, there is no relation between m_iOwnerID and the owner's
    // eHovercraft value, meaning we cannot just directly cast the m_iOwnerID to an eHovercraft
    // TODO deal with Rocket vs Rocket collisions
    if (ENTITY_HOVERCRAFT == pOther->getType() && m_iOwnerID != pOther->getID())
    {
        eHovercraft owner = GAME_STATS->getEHovercraft(m_iOwnerID);
        HovercraftEntity* hit = static_cast<HovercraftEntity*>(pOther);

        // cout << owner << "[ID: " << m_iOwnerID << "] hit " << GAME_STATS->getEHovercraft(hit->getID()) << "[ID: "<< hit->getID() << "]"<< endl;
        // cout << owner << " hit " << GAME_STATS->getEHovercraft(hit->getID()) << endl;

        hit->getHitBy(owner, getAbility());
        handleHovercraftCollision(hit);
    }
}


