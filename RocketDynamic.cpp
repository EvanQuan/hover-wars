#include "RocketDynamic.h"
#include "EntityManager.h"
#include "EmitterEngine.h"
#include "SoundManager.h"

// Default Constructor
RocketDynamic::RocketDynamic(int iID, int iOwnerID)
    : InteractableEntity(iID, iOwnerID, vec3(0.0), ENTITY_ROCKET)
{
    m_pEmitterEngine = EMITTER_ENGINE;
}

// Destructor
RocketDynamic::~RocketDynamic()
{
    // Nothing to destruct
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

void RocketDynamic::initialize(const string& sFileName,
    const ObjectInfo* pObjectProperties,
    const string& sShaderType,
    float fScale, const mat4* m4InitialTransform)
{
    InteractableEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);
    m_iTransformationIndex = m_pMesh->addInstance(m4InitialTransform);
}

void RocketDynamic::update(float fTimeInSeconds)
{
    /* Not Implemented */
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void RocketDynamic::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void Rocket::handleCollision(const Entity* pOther) const
void RocketDynamic::handleCollision(Entity* pOther)
{
    // Tell the Other Entity that they've been hit via the Inherited Collision Handler
    InteractableEntity::handleCollision(pOther);

    // clear Rocket Rendering; Remove Instance from Mesh
}
