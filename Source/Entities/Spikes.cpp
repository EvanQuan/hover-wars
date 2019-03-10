#include "EntityHeaders/Spikes.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
Spikes::Spikes(int iID, int iOwnerID, const vec3* vPosition)
    : InteractableEntity( iID, iOwnerID, *vPosition, ENTITY_SPIKES )
{
    // aLl interactable entities need collision detection
    // m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
}

// Destructor
Spikes::~Spikes()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void Spikes::update(float fTimeInSeconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void Spikes::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void Spikes::handleCollision(const Entity* pOther) const
void Spikes::handleCollision(Entity* pOther)
{
    /* Not Implemented */
}
