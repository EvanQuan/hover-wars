#include "EntityHeaders/Rocket.h"
#include "DataStructures/SpriteSheetDatabase.h"
#include "EntityManager.h"

using namespace SpriteSheetDatabase;

// Default Constructor
Rocket::Rocket(int iID, int iOwnerID, const vec3* vPosition)
    : InteractableEntity( iID, iOwnerID, vPosition, ENTITY_ROCKET )
{
    // aLl interactable entities need collision detection
    // m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
}

// Destructor
Rocket::~Rocket()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void Rocket::update(float fTimeInMilliseconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void Rocket::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void Rocket::handleCollision(const Entity* pOther) const
void Rocket::handleCollision(Entity* pOther)
{
    /* Not Implemented */
}

