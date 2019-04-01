#include "EntityHeaders/Rocket.h"
#include "EntityManager.h"
#include "EmitterEngine.h"
#include "SoundManager.h"
#include "EntityHeaders/HovercraftEntity.h"

// Default Constructor
Rocket::Rocket(int iID, int iOwnerID)
    : InteractableEntity( iID, iOwnerID, vec3(0.0), INTER_ROCKET )
{
    m_pEmitterEngine = EMITTER_ENGINE;
    m_iRocketID = 0;
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

// Updates the Rocket Transformation from the Physics Component
// parameter: fTimeInSeconds - Time in seconds since last update. Not Used.
void Rocket::update(float fTimeInSeconds)
{
    // Local Variables
    mat4 m4TransformationMatrix = mat4(1.0f);

    // Go through each Rocket Reference, grab the Physics Transformation and
    // update the Mesh.
    for (vector<string>::const_iterator pIter = m_pReferenceList.begin();
        pIter != m_pReferenceList.end();
        ++pIter)
    {
        m_pPhysicsComponent->getTransformMatrix(*pIter, &m4TransformationMatrix);
        m_pMesh->updateInstance(&m4TransformationMatrix, *pIter);
    }
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones
// that may have a base mesh or Static position.
void Rocket::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// @Override
void Rocket::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    if (m_iOwnerID != pOther->getID())
    {
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_ROCKET_EXPLOSION);
        // Tell the Other Entity that they've been hit via the Inherited Collision Handler
        InteractableEntity::handleCollision(pOther, iColliderMsg, iVictimMsg);

        removeFromScene(iVictimMsg);
    }
}

void Rocket::handleHovercraftCollision(HovercraftEntity* hit)
{
    cout << "ROCKET HIT PLAYER " << hit->getID() << endl;
}

// clear Rocket Rendering; Remove Instance from Mesh, remove from Physics
void Rocket::removeFromScene(unsigned int iVictimMsg)
{
    string sHashKey = to_string(m_iID) + " " + to_string(iVictimMsg);
    m_pMesh->removeInstance(sHashKey);
    m_pPhysicsComponent->flagForRemoval(sHashKey);
    m_pReferenceList.erase(remove(m_pReferenceList.begin(),
                                  m_pReferenceList.end(),
                                  sHashKey),
                           m_pReferenceList.end());
}

/*************************************************************************************************\
 * Rocket Functionality                                                                          *
\*************************************************************************************************/

void Rocket::launchRocket(const mat4* m4InitialTransform, const vec3* vVelocity, float fBBLength)
{
    // Play sound for Rocket activation
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);

    // Generate Hash Key (<Rocket Entity ID> <Transformation Index>)
    // Transformation index used to differentiate rocket A from rocket B for
    // rendering and physics.
    string sHashKey = to_string(m_iID) + " " + to_string(getNewRocketID());

    // Add Instance to the Mesh for rendering new Rocket
    m_pMesh->addInstance(m4InitialTransform, sHashKey);

    // Save Rocket in Reference Map.
    m_pReferenceList.push_back(sHashKey);

    // Generate Rocket in Physics Scene
    m_pPhysicsComponent->initializeRocket(m_pReferenceList.back().c_str(),
                                          m4InitialTransform, vVelocity, fBBLength);
}

void Rocket::reflect(unsigned int iVictimMsg)
{
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_REFLECT);

    
}
