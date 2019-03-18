#include "EntityHeaders/Powerup.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
Powerup::Powerup(int iID, int iOwnerID, const vec3* vPosition)
    : InteractableEntity( iID, iOwnerID, *vPosition, INTER_POWERUP )
{
    // aLl interactable entities need collision detection
    // m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
}

// Destructor
Powerup::~Powerup()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void Powerup::update(float fTimeInSeconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void Powerup::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void Powerup::handleCollision(const Entity* pOther) const
void Powerup::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    /* Not Implemented */
}
