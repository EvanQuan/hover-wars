#include "EntityHeaders/Rocket.h"
#include "EntityManager.h"
#include "EmitterEngine.h"
#include "SoundManager.h"

// Default Constructor
Rocket::Rocket(int iID, int iOwnerID)
    : InteractableEntity( iID, iOwnerID, vec3(0.0), ROCKET_ENTITY )
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
    // Tell the Other Entity that they've been hit via the Inherited Collision Handler
    InteractableEntity::handleCollision(pOther);

    // clear Rocket Rendering; Remove Instance from Mesh
}

/*************************************************************************************************\
 * Rocket Functionality                                                                          *
\*************************************************************************************************/

void Rocket::launchRocket(const vec3* vPosition, const vec3* vDirection, float fDuration)
{
    cout << "Rocket Launched! Position: " << to_string(*vPosition) << "; Direction: " << to_string(*vDirection) << "\n";
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);
    m_pEmitterEngine->generateEmitter(m_vPosition, vec3(0, 1, 0), 60.f, 5.0f, 5, false, 2.0f);
}

