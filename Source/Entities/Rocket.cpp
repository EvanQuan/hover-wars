#include "EntityHeaders/Rocket.h"
#include "EntityManager.h"
#include "EmitterEngine.h"
#include "SoundManager.h"

// Default Constructor
Rocket::Rocket(int iID, int iOwnerID)
    : InteractableEntity( iID, iOwnerID, vec3(0.0), INTER_ROCKET )
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

void Rocket::launchRocket(const mat4* m4InitialTransform, const vec3* vVelocity, float fBBLength)
{
    // Play sound for Rocket activation
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);

    // Add Instance to the Mesh for rendering new Rocket
    m_iTransformationIndex = m_pMesh->addInstance(m4InitialTransform);

    // Generate Hash Key (<Rocket Entity ID> <Transformation Index>) Transformation index used to differentiate rocket A from rocket B for rendering and physics.
    string sHashKey = to_string(m_iID) + " " + to_string(m_iTransformationIndex);

    // Generate Rocket in Physics Scene
    m_pPhysicsComponent->initializeRocket(sHashKey.c_str(), m4InitialTransform, vVelocity, fBBLength);

    // Save Rocket in Reference Map.
    m_pReferenceMap.insert(make_pair(sHashKey, m_iTransformationIndex));
}

