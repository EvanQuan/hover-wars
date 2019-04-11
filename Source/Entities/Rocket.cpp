#include "EntityHeaders/Rocket.h"
#include "EntityManager.h"
#include "EmitterEngine.h"
#include "SoundManager.h"
#include "EntityHeaders/HovercraftEntity.h"

/***********\
 * DEFINES *
\***********/
#define ANGLE_FROM_NORMAL   360.0f
#define PARTICLE_DURATION   2.0f
#define NUM_PARTICLES       100
#define EXPLOSION_RADIUS    3.0f

int Rocket::LAUNCH_SPEED = 100;

float Rocket::BOUNDING_BOX = 1.0f;

// Default Constructor
Rocket::Rocket(int iID, int iOwnerID, const vec3* vColor)
    : InteractableEntity( iID, iOwnerID, vec3(0.0), INTER_ROCKET )
{
    m_pEmitterEngine    = EMITTER_ENGINE;
    m_iRocketID         = 0;
    m_vExplosionColor   = *vColor;
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
        eEntityType pOtherType = pOther->getType();

        eInteractType pOtherInteractType;
        Rocket *pOtherRocket;
        HovercraftEntity *pOtherHovercraft = nullptr;

        bool shouldReflect = false;
        switch (pOtherType)
        {
        case ENTITY_INTERACTABLE:
            // Tell the rocket to explode.
            pOtherInteractType = static_cast<InteractableEntity*>(pOther)->getInteractableType();
            switch (pOtherInteractType)
            {
            case INTER_ROCKET:
                // If the other entity is also a rocket, tell that rocket to
                // explode as well.
                pOtherRocket = static_cast<Rocket*>(pOther);
                pOtherRocket->explode(iColliderMsg);
                break;
            }
            break;
        case ENTITY_HOVERCRAFT:
            // Normally collision between interacable entities and hovercrafts
            // is done on the hovercraft side. By processing the collision here,
            // we can determine how the rocket is removed from the scene
            // instead of guaranteeing an explosion.
            pOtherHovercraft = static_cast<HovercraftEntity*>(pOther);
            shouldReflect = pOtherHovercraft->hasSpikesActivated();
            break;
        }

        if (shouldReflect && nullptr != pOtherHovercraft) {
            reflect(iVictimMsg, pOtherHovercraft);
        } else {
            // Tell the Other Entity that they've been hit via the Inherited Collision Handler
            InteractableEntity::handleCollision(pOther, iColliderMsg, iVictimMsg);
            explode(iVictimMsg);
        }
    }
}

void Rocket::handleHovercraftCollision(HovercraftEntity *owner, HovercraftEntity *hit)
{
}

// clear Rocket Rendering; Remove Instance from Mesh, remove from Physics
void Rocket::removeFromScene(unsigned int iVictimMsg, bool shouldExplode)
{
    string sHashKey = to_string(m_iID) + " " + to_string(iVictimMsg);
    m_pMesh->removeInstance(sHashKey);

    if (shouldExplode)
    {
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_ROCKET_EXPLOSION);
        mat4 m4FinalTransform;
        m_pPhysicsComponent->getTransformMatrix(sHashKey, &m4FinalTransform);
        m_pEmitterEngine->generateEmitter(m4FinalTransform[3], m4FinalTransform[1], &m_vExplosionColor,
            ANGLE_FROM_NORMAL, PARTICLE_DURATION, NUM_PARTICLES, true, EXPLOSION_RADIUS);
    }

    m_pPhysicsComponent->flagForRemoval(sHashKey);
    m_pReferenceList.erase(remove(m_pReferenceList.begin(),
                                  m_pReferenceList.end(),
                                  sHashKey),
                           m_pReferenceList.end());
}

/*************************************************************************************************\
 * Rocket Functionality                                                                          *
\*************************************************************************************************/

void Rocket::launchRocket(const mat4* m4InitialTransform,
                          const vec3* vVelocity,
                          float fBBLength,
                          bool includeSound)
{
    // Play sound for Rocket activation
    if (includeSound)
    {
        SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);
    }

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

/*
    Remove this rocket from the scene by reflecting. A reflection sound will
    play and a reflected rocket will be silently launched in its place in the
    opposite direction.

    @param iVictimMsg   of rocket to reflect so it can be deleted silently
    @param pOther       hovercraft to reflect rocket, so that the reflected rocket
                        can switch owner.
*/
void Rocket::reflect(unsigned int iVictimMsg, HovercraftEntity *pOther)
{
    string sHashKey = to_string(m_iID) + " " + to_string(iVictimMsg);
    // 1. get rocket's transform and direction
    mat4 transform;
    m_pPhysicsComponent->getTransformMatrix(sHashKey, &transform);
    // vec3 velocity = m_pPhysicsComponent->getDirectionVector(sHashKey);

    // transform[2].y = 0;
    vec3 velocity = glm::normalize(transform[2]) * LAUNCH_SPEED;


    // 2. flip it's direction by 180
    // float angle = glm::radians(180);
    // vec3 axis = vec3(0, 1, 0);
    velocity = -velocity;

    // 3. launch rocket with new direction with the owner's ID without sound
    pOther->reflectRocket(transform, velocity);

    // 4. Silently remove this rocket without explosion
    removeFromScene(iVictimMsg, false);
}


/*
    Remove this rocket from the scene by exploding. An explosion sound will
    play and explosion particles will be emitted in its place.

    @param iVictimMsg   of the rocket to explode
*/
void Rocket::explode(unsigned int iVictimMsg)
{
    removeFromScene(iVictimMsg, true);
}
