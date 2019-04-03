#include "EntityHeaders/Spikes.h"
#include "EntityComponentHeaders/AnimationComponent.h"
#include "SoundManager.h"
#include "EntityManager.h"

/***********\
 * DEFINES *
\***********/
#define RIGHT_SPIKES    0
#define LEFT_SPIKES     1
#define BACK_SPIKES    2
#define FRONT_SPIKES     3
#define RADIANS_90      1.5708f
#define RADIANS_180     3.14159f
#define X_OFFSET        0.65f
#define Z_ORIGIN        -0.5f
#define Z_OFFSET        2.0f
#define ANIMATION_TIME  0.1f

/*************\
 * CONSTANTS *
\*************/
const vec3 UP_VECTOR(0.0f, 1.0f, 0.0f);
const vec3 ORIGIN(0.0f, 0.0f, Z_ORIGIN);
const quat ROTATIONS[NUM_SPIKES] = { angleAxis(RADIANS_90, UP_VECTOR),   /*RIGHT*/
                                     angleAxis(-RADIANS_90, UP_VECTOR),  /*LEFT*/
                                     angleAxis(0.0f, UP_VECTOR),         /*BACK*/
                                     angleAxis(RADIANS_180, UP_VECTOR)}; /*FRONT*/
const vec3 POSITIONS[NUM_SPIKES] = { vec3(-X_OFFSET, 0.0f, Z_ORIGIN),    /*RIGHT*/
                                     vec3(X_OFFSET, 0.0f, Z_ORIGIN),     /*LEFT*/
                                     vec3(0.0f, 0.1f, Z_ORIGIN - Z_OFFSET),    /*BACK*/
                                     vec3(0.0f, -0.25f, Z_ORIGIN + Z_OFFSET) };  /*FRONT*/
const float SCALES[NUM_SPIKES]   = { 0.5f, 0.5f, 0.4f, 0.25f };

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
        m_pSpikeAnimations[i] = ENTITY_MANAGER->generateAnimationComponent(m_iID);
        m_pSpikeAnimations[i]->initializeComponent(m_pMesh);
        m_pSpikeAnimations[i]->addKeyFrame(&ORIGIN, &ROTATIONS[i], 0.0f, ANIMATION_TIME);
        m_pSpikeAnimations[i]->addKeyFrame(&POSITIONS[i], &ROTATIONS[i],
                                           SCALES[i], ANIMATION_TIME);
    }
    
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

// Give the Animation tree the world transformation of the Entity for world translation of the spikes.
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
    InteractableEntity::handleCollision(pOther, iColliderMsg, iVictimMsg);
    m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_IMPACT);
}

/*****************************************************************\
 * Spikes Functionality                                          *
\*****************************************************************/

// Activates Spikes
void Spikes::animateSpikes()
{
    for( unsigned int i = 0; i < NUM_SPIKES; ++i)
        m_pSpikeAnimations[i]->animateToNextFrame();
    m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_ACTIVATE);
}
