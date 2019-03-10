#include "EntityHeaders/Explosion.h"

// Default Constructor
Explosion::Explosion(int iID, int iOwnerID, const vec3* vPosition)
    : InteractableEntity( iID, iOwnerID, vPosition, ENTITY_ROCKET )
{
    // aLl interactable entities need collision detection
    // m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
}

// Destructor
Explosion::~Explosion()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void Explosion::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void Explosion::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void Explosion::handleCollision(const Entity* pOther) const
void Explosion::handleCollision(Entity* pOther)
{
    /* Not Implemented */
}
