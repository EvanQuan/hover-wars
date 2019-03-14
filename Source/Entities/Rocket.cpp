#include "EntityHeaders/Rocket.h"
#include "EntityManager.h"
#include "EmitterEngine.h"
#include "SoundManager.h"

// Default Constructor
Rocket::Rocket(int iID, int iOwnerID)
    : InteractableEntity( iID, iOwnerID, vec3(0.0), ENTITY_ROCKET )
{
    m_pEmitterEngine = EMITTER_ENGINE;
}

// Destructor
Rocket::~Rocket()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

void Rocket::initialize(const string& sFileName,
                        const ObjectInfo* pObjectProperties,
                        const string& sShaderType,
                        float fScale)
{
    InteractableEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);  
}

void Rocket::update(float fTimeInSeconds)
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
    // Tell the Other Entity that they've been hit via the Inherited Collision Handler
    InteractableEntity::handleCollision(pOther);

    // clear Rocket Rendering; Remove Instance from Mesh
}

/*************************************************************************************************\
 * Rocket Functionality                                                                          *
\*************************************************************************************************/

void Rocket::launchRocket(const mat4* m4InitialTransform, float fDuration)
{
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);
    m_iTransformationIndex = m_pMesh->addInstance(m4InitialTransform);
    m_body=  PHYSICS_MANAGER->createRocketObjects(getName(),m4InitialTransform);
}

