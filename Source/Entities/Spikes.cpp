#include "EntityHeaders/Spikes.h"
#include "EntityComponentHeaders/AnimationComponent.h"
#include "SoundManager.h"
#include "EntityManager.h"

/***********\
 * DEFINES *
\***********/
#define NUM_SPIKES      1
#define FRONT_SPIKES    0
#define RADIANS_90      1.5708f
#define POSITION_OFFSET 1.0f
#define ANIMATION_TIME  1.0f

/*************\
 * CONSTANTS *
\*************/
const vec3 UP_VECTOR(0.0f, 1.0f, 0.0f);
const vec3 ORIGIN(0.0f, 0.0f, 0.0f);
const quat ROTATIONS[NUM_SPIKES] = { angleAxis(RADIANS_90, UP_VECTOR) };
const vec3 POSITIONS[NUM_SPIKES] = { vec3(0.0f, 0.0f, POSITION_OFFSET) };
const float SCALES[NUM_SPIKES]   = { 1.0f };

// Default Constructor
Spikes::Spikes(int iID, int iOwnerID)
    : InteractableEntity( iID, iOwnerID, vec3(0.0f), INTER_SPIKES )
{
    m_pSoundMngr = SOUND_MANAGER;
}

// Destructor
Spikes::~Spikes()
{
    // Nothing to destruct
}

/*****************************************************************\
 * Public Spike Functionality                                    *
\*****************************************************************/

// Initialize the Spikes
void Spikes::initialize(const string& sFileName,
                        const ObjectInfo* pObjectProperties,
                        const string& sShaderType,
                        float fScale)
{
    InteractableEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);

    // Initialize Animation Component
    for(unsigned int i = 0; i < NUM_SPIKES; ++i)
    {
        m_pAnimationComponent = ENTITY_MANAGER->generateAnimationComponent(m_iID);
        m_pAnimationComponent->initializeComponent(m_pMesh);
        m_pAnimationComponent->addKeyFrame(&ORIGIN, &ROTATIONS[i], SCALES[i], ANIMATION_TIME);
        m_pAnimationComponent->addKeyFrame(&POSITIONS[i], &ROTATIONS[i],
                                           SCALES[i], ANIMATION_TIME);
        m_pSpikeAnimations.push_back(m_pAnimationComponent);
    }
    
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

void Spikes::update(float fTimeInSeconds)
{
    for (unsigned int i = 0; i < NUM_SPIKES; ++i)
        m_pSpikeAnimations[i]->setWorldTransform(&m_m4WorldTransform);
}

// This will need to be adjusted as needs arise. Particularly for Pick up zones that may have a base mesh or
//  Static position.
void Spikes::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    m_pMesh->getSpatialDimensions(pNegativeCorner, pPositiveCorner);
}

// Overloaded Handle Collision for Spikes
void Spikes::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    InteractableEntity::handleCollision(pOther, iColliderMsg, iVictimMsg );
    m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_IMPACT);
}

/*****************************************************************\
 * Spikes Functionality                                          *
\*****************************************************************/

// Activates Spikes
void Spikes::animateSpikes()
{
    m_pAnimationComponent->animateToNextFrame();
    m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_ACTIVATE);
}
