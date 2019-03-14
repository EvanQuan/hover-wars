#include "EntityHeaders/InteractableEntity.h"
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
    m_pMesh             = MESH_MANAGER->loadMeshFromFile(&m_iTransformationIndex, sFileName, pObjectProperties, fScale);
    m_pRenderComponent  = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);
    m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
}

void InteractableEntity::update(float fTimeInSeconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void InteractableEntity::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// Name: handleCollision
// Written by: James Cote
// Description: Default functionality for Interactable Entities.
//      Default: tells the other entity that they've been hit.
void InteractableEntity::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg, PxVec3 vNormal, PxVec3 vPosition)
{
    if (ENTITY_HOVERCRAFT == pOther->getType())
        static_cast<HovercraftEntity*>(pOther)->getHitBy(m_eType, static_cast<eHovercraft>(m_iOwnerID));
}


