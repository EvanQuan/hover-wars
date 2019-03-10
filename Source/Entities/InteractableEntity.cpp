#include "EntityHeaders/InteractableEntity.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
InteractableEntity::InteractableEntity(int iID, int iOwnerID, const vec3* vPosition, eEntityType eIEType)
    : Entity( iID, *vPosition, eIEType )
{
    m_iOwnerID = iOwnerID;  // Set the Owner ID for the Interactable Entity
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
void InteractableEntity::handleCollision(Entity* pOther)
{
    if (ENTITY_HOVERCRAFT == pOther->getType())
        static_cast<HovercraftEntity*>(pOther)->getHitBy(m_eType, m_iOwnerID);
}


