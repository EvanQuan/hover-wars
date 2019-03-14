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
    mat4 m4TransformationMatrix = mat4(1.0f);

    for (unordered_map<string, unsigned int>::const_iterator pIter = m_pReferenceMap.begin();
        pIter != m_pReferenceMap.end();
        ++pIter)
    {
        m_pPhysicsComponent->getTransformMatrix(pIter->first, &m4TransformationMatrix);
        m_pMesh->updateInstance(&m4TransformationMatrix, pIter->second);
    }
    
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void Rocket::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    /* Not Implemented */
}

// void Rocket::handleCollision(const Entity* pOther) const
void Rocket::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg, PxVec3 vNormal, PxVec3 vPosition)
{
    if (m_iOwnerID != pOther->getID())
    {
        // Tell the Other Entity that they've been hit via the Inherited Collision Handler
        InteractableEntity::handleCollision(pOther, iColliderMsg, iVictimMsg, vNormal, vPosition);

        // clear Rocket Rendering; Remove Instance from Mesh
        m_pMesh->removeInstance(iVictimMsg);
        string sHashKey = to_string(m_iID) + " " + to_string(iVictimMsg);
        m_pPhysicsComponent->flagForRemoval(sHashKey);
        m_pReferenceMap.erase(sHashKey);
        
    }
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

    // Save Rocket in Reference Map.
    m_pReferenceMap.insert(make_pair(sHashKey, m_iTransformationIndex));
    unordered_map<string, unsigned int>::iterator pIter = m_pReferenceMap.find(sHashKey);

    // Generate Rocket in Physics Scene
    m_pPhysicsComponent->initializeRocket(pIter->first.c_str(), m4InitialTransform, vVelocity, fBBLength);
}

