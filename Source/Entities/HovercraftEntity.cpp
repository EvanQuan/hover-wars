#include "EntityHeaders/HovercraftEntity.h"
#include "EntityHeaders/FlameTrail.h"
#include "EntityHeaders/Rocket.h"
#include "EntityComponentHeaders/AnimationComponent.h"
#include "SceneLoader.h"
#include "MeshManager.h"
#include "EntityManager.h"
#include "SoundManager.h"

/***********\
 * Defines *
\***********/

// Ability Defines
/*
    Rocket
    Spikes
    Trail
    Dash - (all 4 directions count as 1 ability for cool down purposes)
*/
#define ABILITY_COUNT           COOLDOWN_COUNT
/*
    Unit : meters / second
*/
#define FLAME_SPACING           1.0f

// @Deprecated - Hovercrafts don't lose control
#define LOSE_CONTROL_COLLISION_TIME 1.0f // 0.8
/*
    If the hovercraft exceeds a certain elevant, they lose control until they
    return below it. This prevents hovercrafts from continuing to apply a
    driving force when they are in the air.
*/
#define LOSE_CONTROL_COLLISION_ELEVATION 2.3f

/*
    If a hovercraft successfully hits another hovercraft with a rocket,
    the rocket cooldown will be reduced by this multiplier value.

    This rewards higher rocket accuracy instead of spamming.
*/
#define COOLDOWN_REDUCTION_ON_HIT 0.5f

/*
    Cooldowns

    The time the hovercraft must wait until they can use the ability again.
    These are the base cooldowns that all hovercrafts start with, and so
    should be a bit on the long end.

    Units: seconds
*/
#define ROCKET_BASE_COOLDOWN        6.0f
#define SPIKES_BASE_COOLDOWN        6.0f
#define TRAIL_COOLDOWN              0.0f
// Cooldown between dash usages
#define DASH_BASE_COOLDOWN          0.5f
// Time to gain a dash charge
#define DASH_BASE_RECHARGE          4.0f

#define DASH_MAX_CHARGE_COUNT       3

/*
    Once spikes are activated, they are enabled for a duration before deactivating.
*/
#define SPIKES_DURATION         1.0f

/*
Power up cooldowns

These are the minimum cooldowns for these abilities.

*/
#define ROCKET_MIN_COOLDOWN 2.0f
#define SPIKES_MIN_COOLDOWN 2.0f
#define DASH_MIN_RECHARGE   1.0f

/*
    Multiplies all cool down values to decrease them. Should be below 1.0
*/
#define MAX_COOLDOWN_REDUCTION_ON_KILL 0.9f

/*
    Multiplies the trail recharge multiplier when max cooldown reduction is
    applied.
*/
#define TRAIL_RECHARGE_INCREASE 1.1f
/*
Total time the trail can be activated from full to empty.

Unit: seconds
*/
#define TRAIL_GAUGE_FULL        3.0f
/*
Represents the trail gauge is empty.
*/
#define TRAIL_GAUGE_EMPTY       0.0f

// Fire dimensions
// Affects both the hitboxes and sprite size
#define FIRE_HEIGHT             3.0
#define FIRE_WIDTH              3.0

/*
    Duration powerups last before disapating.
*/
#define POWERUP_DURATION        15.0f

/*
The maximum speed the player can normally travel. This ensures the player
does not infinitely accelerate as they move.

Speed : meters/second
*/
#define MAX_NORMAL_SPEED    30
#define MAX_POWERUP_SPEED   60

/*
Time multiplier for the trail to recharge from empty to full.

= 1: recharge rate is the same as drain rate.
> 1: recharge rate is faster than drain rate
< 1: recharge rate is slower than drain rate

*/
#define TRAIL_BASE_RECHARGE_MULTIPLIER 0.3f // 9 sec

#define TRAIL_MAX_RECHARGE_MULTIPLIER 1.0f  // 3 sec

/*
The interval of time between each created flame while the trail trail is
activated.

@TODO Flame interval should be based on distance, not time. In some sense, a
line is simply being laid out, of which flame billboards are uniformly
distributed across, meanining that the spacing is time invariant.

Unit: seconds
*/
#define FLAME_INTERVAL          0.10f

/*
Delay time when the trail is deactivate and when the gauge begins to recharge.
This makes spam toggling less effective.

Unit: seconds
*/
#define TRAIL_RECHARGE_COOLDOWN 0.5f

/*
After getting hit, the hovercraft is invulnerable for a duration of time

Unit : seconds
*/
#define INVINCIBLE_TIME 1.0f

/*
Determines from what horizontal angle the camera is tracking the hovercraft.
                   90
                  -270
                    |
                    v
     180 -----> hovercraft  <----- 0
    -180            ^              360
                    |
                   270
                   -90

The front camera should be facing the hovercraft's backside.

theta : degrees
*/
#define FRONT_CAMERA_LONGITUDE  180.0f

/*
Determines the vertical angle the camera is tracking the hovercraft.
          45      0
            \     |
             \    v
    90 -----> hovercraft

The front camera should be low enough to give the player as much vision ahead
of them as possible. This will give more information on where to drive, as well
as make aiming the rocket easier.

phi : degrees
*/
#define FRONT_CAMERA_LATITUDE   80.0f

/*
Distance between the centre of the car and the look-at position.

If negative, the camera will look behind the car's centre.
*/
#define FRONT_CAMERA_OFFSET     0       // r        meters

/*
Distance between look-at position and camera.
*/
#define FRONT_CAMERA_RADIUS     10.0f   // r        meters
#define BACK_CAMERA_LONGITUDE   -FRONT_CAMERA_LONGITUDE  // theta    degrees

/*
This determines the pitch that the camera
*/
#define BACK_CAMERA_LATITUDE    -FRONT_CAMERA_LATITUDE   // phi      degrees
// #define BACK_CAMERA_LATITUDE    40.0f   // phi      degrees

/*
Distance between look-at position and camera.
*/
#define BACK_CAMERA_RADIUS      12.0f   // r        meters
/*
Distance between the centre of the car and the look-at position.

If negative, the camera will look behind the car's centre.
*/
#define BACK_CAMERA_OFFSET      -10     // -10 r        meters

// Camera Spring Constants
/*
This should always be zero.

Units: meters
*/
#define CAMERA_REST_LENGTH 0.0f
/*
The larger the spring constant, the stronger the spring effect.
In other words, the spring will pull together faster the higher the constant is.
*/
#define SPRING_MOVEMENT_CONSTANT 50.0f  // unitless
#define SPRING_ROTATION_CONSTANT 20.0f  // unitless

/**********************\
 * SPIKES INFORMATION *
\**********************/
/***********\
 * DEFINES *
\***********/
#define RIGHT_SPIKES    0
#define LEFT_SPIKES     1
#define BACK_SPIKES    2
#define FRONT_SPIKES     3
#define RADIANS_90      1.5708f
#define RADIANS_180     3.14159f
#define X_OFFSET        0.5f
#define Y_OFFSET        -0.05f
#define Z_ORIGIN        -0.5f
#define Z_BACK_OFFSET   2.05f
#define Z_FRONT_OFFSET  2.0f
#define ANIMATION_TIME  0.1f

// Threshold of the y-component (vertical) of the hovercraft direction vector
// that will reset the hovercraft's y-component if it is exceeded.
#define HORIZONTAL_DEVIATION_THRESHOLD 0.1

/*************\
 * CONSTANTS *
\*************/
const vec3 UP_VECTOR(0.0f, 1.0f, 0.0f);
const vec3 ORIGIN(0.0f, 0.0f, Z_ORIGIN);
const quat ROTATIONS[NUM_SPIKES] = { angleAxis(RADIANS_90, UP_VECTOR),   /*RIGHT*/
                                     angleAxis(-RADIANS_90, UP_VECTOR),  /*LEFT*/
                                     angleAxis(0.0f, UP_VECTOR),         /*BACK*/
                                     angleAxis(RADIANS_180, UP_VECTOR) }; /*FRONT*/
const vec3 POSITIONS[NUM_SPIKES] = { vec3(-X_OFFSET, Y_OFFSET, Z_ORIGIN),    /*RIGHT*/
                                     vec3(X_OFFSET, Y_OFFSET, Z_ORIGIN),     /*LEFT*/
                                     vec3(0.0f, 0.1f, Z_ORIGIN - Z_BACK_OFFSET),    /*BACK*/
                                     vec3(0.0f, -0.25f, Z_ORIGIN + Z_FRONT_OFFSET) };  /*FRONT*/
const float SCALES[NUM_SPIKES] = { 0.5f, 0.5f, 0.4f, 0.25f };


const vec3 FRONT_CAMERA_START_VIEW = vec3(FRONT_CAMERA_LONGITUDE, FRONT_CAMERA_LATITUDE, FRONT_CAMERA_RADIUS); // (Theta, Phi, Radius)
const vec3 BACK_CAMERA_START_VIEW = vec3(BACK_CAMERA_LONGITUDE, BACK_CAMERA_LATITUDE, BACK_CAMERA_RADIUS); // (Theta, Phi, Radius)
/*
The position of the camera relative to the position of the player. Both vectors
will be added together to form the final camera position.
*/
const vec3 FRONT_CAMERA_POSITION_OFFSET = vec3(0, 0, FRONT_CAMERA_OFFSET);
const vec3 BACK_CAMERA_POSITION_OFFSET = vec3(0, 0, BACK_CAMERA_OFFSET);

// Hovercraft Entity Default Constructor.
//  Call the base class Entity Constructor
HovercraftEntity::HovercraftEntity(int iID, const vec3* vPosition)
    : Entity(iID, *vPosition, ENTITY_HOVERCRAFT)
{
    // Initialize base information.
    m_pSpatialMap               = SPATIAL_DATA_MAP;
    m_pGameStats                = GAME_STATS;
    m_pSoundMngr                = SOUND_MANAGER;
}

HovercraftEntity::~HovercraftEntity()
{

}

void HovercraftEntity::reinitialize()
{
    activeCameraIndex           = FRONT_CAMERA;
    m_qCurrentCameraRotation    = quat();
    m_vCurrentCameraPosition    = vec3(0.0f);

    m_bSpikesActivated = false;

    m_bInvincible = false;

    outOfControlTime = 0.0f;

    m_iDashMaxCharges = DASH_MAX_CHARGE_COUNT;

    initializeCooldowns();
    initializePowerups();
}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

/*
    @param fTimeInSeconds   delta time since last update
*/
void HovercraftEntity::update(float fTimeInSeconds)
{
    lowEnoughToMove = m_pPhysicsComponent->getPosition().y < LOSE_CONTROL_COLLISION_ELEVATION;

    // New Transformation Matrix
    mat4 m4NewTransform = mat4(1.0f);

    // Get the Transformation from the Physics component
    m_pPhysicsComponent->getTransformMatrix(&m4NewTransform);

    // Spike Animations
    for (unsigned int i = 0; i < NUM_SPIKES; ++i)
        m_pSpikeAnimations[i]->setWorldTransform(&m4NewTransform);

    // If there's a new Transformation, apply it to the Mesh.
    m_pMesh->updateInstance(&m4NewTransform, m_sName);

    // Check to update Dynamic Position in Spatial Map
    vec3 vNewPosition = m4NewTransform[3];

    updateSpatialMap(vNewPosition);

    updatePhysicsComponent();

    // Calculate Position Averages for Camera
    m_vPosition = vNewPosition;
    updateInControl(fTimeInSeconds);
    updateCameraLookAts(fTimeInSeconds);
    updateCooldowns(fTimeInSeconds);
    updateVulnerability(fTimeInSeconds);
    updatePowerups(fTimeInSeconds);
    updateQueuedActions();
}

void HovercraftEntity::updateSpatialMap(vec3 &vNewPosition)
{
    if (m_vPosition != vNewPosition)
    {
        m_pSpatialMap->updateDynamicPosition(this, &vNewPosition);
    }
}

/*
    Update the location of the physics component.
*/
void HovercraftEntity::updatePhysicsComponent()
{
    resetIfNotHorizontal();
}

/*
    If the y-component (vertical) that the Hovercraft is facing deviates too
    far from horizontal, the global position is reset such that it back to
    horizontal. This prevents hovercrafts from riding up walls if they
    constantly push against walls, or climbing on other hovercrafts when they
    collide.

    @NOTE currently bugged. Resets x and z component of rotation (in cartesian
          coordinates) as well.
*/
void HovercraftEntity::resetIfNotHorizontal()
{
    vec3 dirVector;
    getDirectionVector(&dirVector);

    // Check if not horizontal.
    if (abs(dirVector.y) > HORIZONTAL_DEVIATION_THRESHOLD) {
        //TODO set quat for rotation
        vec3 newQuatAxis = vec3(dirVector.x,
                                0.1f * (dirVector.y/abs(dirVector.y)),
                                dirVector.z);

        PxPlane plane = PxPlane(PxVec3(0,0,0),
                                PxVec3(newQuatAxis.x, newQuatAxis.y, newQuatAxis.z),
                                PxVec3(newQuatAxis.z, 0, -newQuatAxis.x));

        PxTransform newTrans = getGlobalTransform();

        // cout << newTrans.q.x << " " << newTrans.q.y << " " << newTrans.q.z << " " << newTrans.q.w;
        // cout << newTrans.q.y << endl;

        // newTrans.q = PxQuat(newTrans.q.getNormalized().getAngle(), plane.n.getNormalized());
        newTrans.q = PxQuat(0, plane.n.getNormalized());

        // Original. 
        // newTrans.q = PxQuat(0, plane.n);

        m_pPhysicsComponent->setGlobalPos(newTrans);
    }

}
void HovercraftEntity::setPosition(vec2 pos) {
    m_pPhysicsComponent->setPosition(pos);
}
void HovercraftEntity::updateQueuedActions()
{
    /* Currently coded for only speed power up */
    if (queuedActions[QUEUED_SPEED_BOOST])
    {
        enablePowerup(ePowerup::POWERUP_SPEED_BOOST);
        queuedActions[QUEUED_SPEED_BOOST] = false;
    }
    if (queuedActions[QUEUED_PUSH])
    {
        push(queuedX, queuedY);
        queuedActions[QUEUED_PUSH] = false;
    }
}

/*
    Tells the HovercraftEntity that they were damaged. This is where the
    Hovercraft Entity will handle its "death" logic and award points to the
    attacker.

    @param attacker         to award points to
    @param ability          that hit this. An invalid ability will still
                            register a hit, but not increment any ability
                            kill stats.
*/
void HovercraftEntity::getHitBy(eHovercraft attacker, eAbility ability)
{
    if (isInvincible() || (ability == eAbility::ABILITY_TRAIL_ACTIVATE && isDashing()) ) {
        return;
    }
    HovercraftEntity* attackerHovercraft = ENTITY_MANAGER->getHovercraft(attacker);
    bool attackerHadLargestScore = m_pGameStats->hasLargestScore(attackerHovercraft->getEHovercraft());
    if (m_pGameStats->hasLargestScore(m_eHovercraft))
    {
        // If the attacker hit the hovercraft with the largest score, give them
        // a speed boost
        attackerHovercraft->queuePowerup(ePowerup::POWERUP_SPEED_BOOST);
    }
    setInvincible();
    m_pGameStats->addScore(attacker,
                           static_cast<GameStats::eAddScoreReason>(m_eHovercraft),
                           ability);
    resetMaxCooldowns();

    // Award attacker
    attackerHovercraft->reduceMaxCooldowns();
    attackerHovercraft->reduceCooldown(ability);

    switch (ability)
    {
    case ABILITY_TRAIL_ACTIVATE:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_TRAIL_IMPACT);
        break;
    }
}

/*
    Permanently reduce the max cooldowns of all abilities.
*/
void HovercraftEntity::reduceMaxCooldowns()
{
    m_fMaxCooldowns[COOLDOWN_ROCKET] = FuncUtils::max(ROCKET_MIN_COOLDOWN,
                                                      m_fMaxCooldowns[COOLDOWN_ROCKET] * MAX_COOLDOWN_REDUCTION_ON_KILL);
    m_fMaxCooldowns[COOLDOWN_SPIKES] = FuncUtils::max(SPIKES_MIN_COOLDOWN,
                                                      m_fMaxCooldowns[COOLDOWN_SPIKES] * MAX_COOLDOWN_REDUCTION_ON_KILL);
    m_fTrailRechargeMultipler = FuncUtils::min(TRAIL_MAX_RECHARGE_MULTIPLIER,
                                               m_fTrailRechargeMultipler * TRAIL_RECHARGE_INCREASE);
    // m_fMaxCooldowns[COOLDOWN_DASH]   = FuncUtils::max(DASH_MIN_COOLDOWN,
    //                                                   m_fMaxCooldowns[COOLDOWN_DASH] * MAX_COOLDOWN_REDUCTION_ON_KILL);
    m_fDashMaxRecharge = FuncUtils::max(DASH_MIN_RECHARGE,
                                        m_fDashMaxRecharge * MAX_COOLDOWN_REDUCTION_ON_KILL);
}

void HovercraftEntity::reduceCooldown(eAbility ability)
{
    // Right now we only will use rockets and spikes
    switch (ability)
    {
    case ABILITY_ROCKET:
        m_fCooldowns[COOLDOWN_ROCKET] *= COOLDOWN_REDUCTION_ON_HIT;
        break;
    case ABILITY_SPIKES:
        m_fCooldowns[COOLDOWN_SPIKES] *= COOLDOWN_REDUCTION_ON_HIT;
        break;
    }
}

void HovercraftEntity::resetMaxCooldowns()
{
    m_fMaxCooldowns[COOLDOWN_ROCKET] = ROCKET_BASE_COOLDOWN;
    m_fMaxCooldowns[COOLDOWN_SPIKES] = SPIKES_BASE_COOLDOWN;
    // Dash cooldown is constant. It is the dash recharge that is altered.
    // m_fMaxCooldowns[COOLDOWN_DASH]   = DASH_BASE_COOLDOWN;
    m_fDashMaxRecharge = DASH_BASE_RECHARGE;
}

/*
    Update the vulnerability status.
    The hovercraft is only invulnerable for a duration until they are
    vulernable again.
*/
void HovercraftEntity::updateVulnerability(float fTimeInSeconds)
{
    m_fSecondsLeftUntilVulnerable -= fTimeInSeconds;
    if (m_fSecondsLeftUntilVulnerable <= 0)
    {
        m_bInvincible = false;
    }
}

/*
    Update the powerup statuses.
    Each powerup only lasts for a duration until they expire, losing
    the effect.
*/
void HovercraftEntity::updatePowerups(float fTimeInSeconds)
{
    for (int powerup = 0; powerup < POWERUP_COUNT; powerup++)
    {
        m_vPowerupsTime[powerup] -= fTimeInSeconds;
        if (m_vPowerupsEnabled[powerup] && (m_vPowerupsTime[powerup] <= 0))
        {
            disablePowerup(static_cast<ePowerup>(powerup));
        }
    }
}

/*
    Enable a powerup. Once enabled, the powerup provides benefits to the
    hovercraft.

    @param powerup  to enable
*/
void HovercraftEntity::enablePowerup(ePowerup powerup)
{
    switch (powerup)
    {
    case POWERUP_SPEED_BOOST:
        m_pPhysicsComponent->setMaxSpeed(MAX_POWERUP_SPEED);
        //cout << m_eHovercraft << " speed boost enabled" << endl;
        m_pSoundMngr->play(SoundManager::SOUND_POWERUP_SPEED_ACTIVATE, m_bIsPlayer);
        break;
    case POWERUP_ROCKET_COOLDOWN:
        m_fMaxCooldowns[COOLDOWN_ROCKET] = ROCKET_MIN_COOLDOWN;
        break;
    case POWERUP_SPIKES_COOLDOWN:
        m_fMaxCooldowns[COOLDOWN_SPIKES] = SPIKES_MIN_COOLDOWN;
        break;
    case POWERUP_DASH_RECHARGE:
        // m_fMaxCooldowns[COOLDOWN_DASH] = DASH_MIN_COOLDOWN;
        break;
    default:
        return;
    }
    m_vPowerupsEnabled[powerup] = true;
    m_vPowerupsTime[powerup] = POWERUP_DURATION;
    GAME_STATS->addScore(m_eHovercraft, GameStats::PICKUP_POWERUP);
}

/*
    Queues up a powerup to activate for the next frame update.
    For now, coded to speed boost only

    @param powerup  to queue
*/
void HovercraftEntity::queuePowerup(ePowerup powerup)
{
    switch (powerup)
    {
    case POWERUP_SPEED_BOOST:
        queuedActions[QUEUED_SPEED_BOOST] = true;
        break;
    }
}

/*
    Queues up a push for the next frame update

    @param x    x-coordinate of push in world-space
    @param y    y-coordinate of push in world-space
*/
void HovercraftEntity::queuePush(float x, float y)
{
    queuedX = x;
    queuedY = y;
    queuedActions[QUEUED_PUSH] = true;
}

/*
    Disable a powerup. The powerup effect is lost.

    @param powerup  to disable
*/
void HovercraftEntity::disablePowerup(ePowerup powerup)
{
    switch (powerup)
    {
    case POWERUP_SPEED_BOOST:
        m_pPhysicsComponent->setMaxSpeed(MAX_NORMAL_SPEED);
        //cout << GAME_STATS->getEHovercraft(m_iID) << " speed boost disabled" << endl;
        m_pSoundMngr->play(SoundManager::SOUND_POWERUP_SPEED_DEACTIVATE, m_bIsPlayer);
        break;
    case POWERUP_ROCKET_COOLDOWN:
        m_fMaxCooldowns[COOLDOWN_ROCKET] = ROCKET_BASE_COOLDOWN;
        break;
    case POWERUP_SPIKES_COOLDOWN:
        m_fMaxCooldowns[COOLDOWN_SPIKES] = SPIKES_BASE_COOLDOWN;
        break;
    case POWERUP_DASH_RECHARGE:
        // m_fMaxCooldowns[COOLDOWN_DASH] = DASH_BASE_COOLDOWN;
        break;
    default:
        return;
    }
    m_vPowerupsEnabled[powerup] = false;
}

// Fetches the Spatial Dimensions of the Mesh/Bounding Box if applicable.
void HovercraftEntity::getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const
{
    m_pMesh->getSpatialDimensions(pNegativeCorner, pPositiveCorner);
}

// Initializes Entity information
void HovercraftEntity::initialize(const string& sFileName,
                                  const ObjectInfo* pObjectProperties,
                                  const string& sShaderType,
                                  const vec3* vColor,
                                  float fScale)
{
    // Load Mesh and Rendering Component
    EntityManager* pEntityMngr = ENTITY_MANAGER;
    ShaderManager::eShaderType eShader = SHADER_MANAGER->getShaderType(sShaderType);
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pObjectProperties, m_sName, fScale);
    m_pSpikesMesh = SCENE_LOADER->createSpikesMesh(m_sName);
    m_pRenderComponent = pEntityMngr->generateRenderComponent(m_iID, m_pMesh, true, eShader, GL_TRIANGLES);
    m_pSpikesRenderComponent = pEntityMngr->generateRenderComponent(m_iID, m_pSpikesMesh, true, eShader, GL_TRIANGLES);
    m_vPosition = pObjectProperties->vPosition;

    vec3 vNegCorner, vPosCorner;
    getSpatialDimensions(&vNegCorner, &vPosCorner);
    ObjectInfo::BoundingBox sBounding;
    sBounding.vDimensions = vPosCorner - vNegCorner;

    // PHYSICSTODO: Set up Physics Component as a Dynamic Physics Object for a player
    m_pPhysicsComponent = pEntityMngr->generatePhysicsComponent(m_iID);
    m_pPhysicsComponent->initializeVehicle(getName(),
                                           true,
                                           &sBounding,
                                           pObjectProperties->vPosition,
                                           MAX_NORMAL_SPEED);

    // Set up Mesh for Initial Transformation drawing.
    mat4 m4InitialTransform;
    m_pPhysicsComponent->getTransformMatrix(&m4InitialTransform);
    m_pMesh->addInstance(&m4InitialTransform, m_sName);

    // The fire trail entity is always at the same location as the hovecraft
    m_pFireTrail = pEntityMngr->generateFlameTrailEntity(&m_vPosition, vColor, m_iID,
                                                         FIRE_HEIGHT, FIRE_WIDTH);
    m_pFireTrail->initialize();

    // Create Rocket Mesh
    m_pRocket = SCENE_LOADER->createRocketMesh(m_iID, vColor);

    // Initialize Spikes Animations
    for (unsigned int i = 0; i < NUM_SPIKES; ++i)
    {
        m_pSpikeAnimations[i] = ENTITY_MANAGER->generateAnimationComponent(m_iID);
        m_pSpikeAnimations[i]->initializeComponent(m_pSpikesMesh);
        m_pSpikeAnimations[i]->addKeyFrame(&ORIGIN, &ROTATIONS[i], 0.0f, ANIMATION_TIME);
        m_pSpikeAnimations[i]->addKeyFrame(&POSITIONS[i], &ROTATIONS[i],
                                           SCALES[i], ANIMATION_TIME);
    }
    // Generate Camera Components
    for (unsigned int i = 0; i < MAX_CAMERAS_PER_PLAYER; ++i)
    {
        m_pCmrComponents[i] = pEntityMngr->generateCameraComponent(m_iID);
        m_pCmrComponents[i]->setLookAt(m_vPosition);
    }

    m_pCmrComponents[FRONT_CAMERA]->setSphericalPos(FRONT_CAMERA_START_VIEW);
    m_pCmrComponents[BACK_CAMERA]->setSphericalPos(BACK_CAMERA_START_VIEW);

    reinitialize();
}

/*
    Handle Collision Logic in this function. This function is called when
    someone collides with this Entity. This Entity can tell the other Entity
    what happens when they collided with this Entity.

    @param pOther   const pointer to the Entity that this entity collided with.
    @param bVictim  boolean to tell if this entity is the victim or not.
*/
void HovercraftEntity::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    // Get the Type of the Other Entity
    eEntityType eOtherType = pOther->getType();
    // eInteractType eOtherInteractableType;
    HovercraftEntity* pOtherHovercraft;
    // InteractableEntity* pOtherInteractable;
    switch (eOtherType)
    {
    case ENTITY_HOVERCRAFT:
        // Cast the other Entity to a Hovercraft Entity (We know this is
        // possible because of the two cases)
        pOtherHovercraft = static_cast<HovercraftEntity*>(pOther);
        if (m_bSpikesActivated)
        {   // Tell the Targetted Entity that they were hit by this bot.
           pOtherHovercraft->getHitBy(GAME_STATS->getEHovercraft(m_iID), ABILITY_SPIKES);
           m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_IMPACT);
           reduceMaxCooldowns();
        }
        if (pOtherHovercraft->hasSpikesActivated())
        {
            this->getHitBy(GAME_STATS->getEHovercraft(pOtherHovercraft->getID()), ABILITY_SPIKES);
            m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_IMPACT);
        }
        // Momentarily lose control of vehicle to prevent air moving
    
        // TODO check if need to determine which velocity to do

        
        // vec3 myVelocity = getLinearVelocity();
        // vec3 otherVelocity = pOtherHovercraft->getLinearVelocity();
        // float mySpeed = glm::length(myVelocity);
        // float otherSpeed = glm::length(otherVelocity);

        // Multithread issues, can't do push directly
        // Disabled for now until rotation collision bug is fixed
        // if (mySpeed > otherSpeed) {
            // pOtherHovercraft->setLoseControl(LOSE_CONTROL_COLLISION_TIME);
            // pOtherHovercraft->queuePush(myVelocity.x, myVelocity.y);
        // } else {
            // setLoseControl(LOSE_CONTROL_COLLISION_TIME);
            // queuePush(otherVelocity.x, otherVelocity.y);
        // }

        break;
    }
}

/*
    Set the duration for the hovercraft to ignore input.

    @param seconds  until control is regained
*/
void HovercraftEntity::setLoseControl(float seconds)
{
    outOfControlTime = seconds;
    inControl = false;
    // cout << m_iID << " Lost control" << endl;
}


/********************************************************************************************************\
 * Private Functions                                                                                    *
\********************************************************************************************************/

/*
    Initialize all ability cooldowns and fills the flame trail gauge to full.
*/
void HovercraftEntity::initializeCooldowns()
{
    for (int ability = 0; ability < ABILITY_COUNT; ability++)
    {
        m_fCooldowns[ability] = 0.0f;
    }

    m_fMaxCooldowns[COOLDOWN_ROCKET] = ROCKET_BASE_COOLDOWN;
    m_fMaxCooldowns[COOLDOWN_SPIKES] = SPIKES_BASE_COOLDOWN;
    m_fMaxCooldowns[COOLDOWN_TRAIL_ACTIVATE] = TRAIL_COOLDOWN;
    m_fMaxCooldowns[COOLDOWN_TRAIL_DEACTIVATE] = TRAIL_COOLDOWN;
    m_fMaxCooldowns[COOLDOWN_DASH]   = DASH_BASE_COOLDOWN;

    m_fTrailGauge = TRAIL_GAUGE_FULL;
    m_fSecondsSinceLastFlame = 0.0f;
    m_fTrailRechargeMultipler = TRAIL_BASE_RECHARGE_MULTIPLIER;

    m_iDashCharges = DASH_MAX_CHARGE_COUNT;
    m_fDashMaxRecharge = DASH_BASE_RECHARGE;
    m_fDashRecharge = DASH_BASE_RECHARGE;

}

/*
    Disables all powerups.
*/
void HovercraftEntity::initializePowerups()
{
    for (int powerup = 0; powerup < POWERUP_COUNT; powerup++)
    {
        m_vPowerupsTime[powerup] = 0;
        m_vPowerupsEnabled[powerup] = false;
    }
}

/*
    Updates the location for this hovercraft's cameras. This is what makes the
    camera sway as the hovercraft moves to make it seem more natural.
*/
void HovercraftEntity::updateCameraLookAts(float fTimeInSeconds)
{
    updateCameraRotation(fTimeInSeconds);
    updateCameraPosition(fTimeInSeconds);
}

/*
    Ensure the camera is looking at the hovercraft
*/
void HovercraftEntity::updateCameraRotation(float fTimeInSeconds)
{
    quat qCurrRotation = m_pPhysicsComponent->getRotation();
    if (qCurrRotation != m_qCurrentCameraRotation)
    {
        quat cameraRotationDirection = qCurrRotation - m_qCurrentCameraRotation;
        float fSpring = SPRING_ROTATION_CONSTANT * (length(cameraRotationDirection) - CAMERA_REST_LENGTH);

        m_qCurrentCameraRotation += (normalize(cameraRotationDirection) * fSpring) * fTimeInSeconds;

        m_pCmrComponents[FRONT_CAMERA]->setRotationQuat(m_qCurrentCameraRotation);
        m_pCmrComponents[BACK_CAMERA]->setRotationQuat(m_qCurrentCameraRotation);
    }
}

/*
    Ensure the camera is close enough to the hovercraft
*/
void HovercraftEntity::updateCameraPosition(float fTimeInSeconds)
{
    vec3 cameraLength = m_vPosition - m_vCurrentCameraPosition;

    if (vec3(0.0f) != cameraLength) // Don't proceed if there's no change.
    {
        float fSpring = SPRING_MOVEMENT_CONSTANT * (length(cameraLength) - CAMERA_REST_LENGTH);
        m_vCurrentCameraPosition += (normalize(cameraLength) * fSpring) * (fTimeInSeconds);

        // Update all the camera look at and rotation values based on the averaging calculations.
        m_pCmrComponents[FRONT_CAMERA]->setLookAt(m_vCurrentCameraPosition + m_qCurrentCameraRotation * FRONT_CAMERA_POSITION_OFFSET);
        m_pCmrComponents[BACK_CAMERA]->setLookAt(m_vCurrentCameraPosition + m_qCurrentCameraRotation * BACK_CAMERA_POSITION_OFFSET);
    }
}

/*
    Hovercrafts only lose control after a duration. If the duration is exceed,
    regain control.
*/
void HovercraftEntity::updateInControl(float fTimeInSeconds)
{
    if (!inControl)
    {
        outOfControlTime -= fTimeInSeconds;
        if (outOfControlTime <= 0)
        {
            inControl = true;
            // cout << m_iID << " gained control" << endl;
        }
    }
}

/*
    This will decrease the cooldown value all all abilities by the time that
    has passed.
*/
void HovercraftEntity::updateCooldowns(float fTimeInSeconds)
{
    for (int i = 0; i < ABILITY_COUNT; i++)
    {
        if (m_fCooldowns[i] > 0)
        {
            // Ensure that cooldown values are never negative
            float newCooldown = m_fCooldowns[i] - fTimeInSeconds;
            if (newCooldown > 0.0f)
            {
                m_fCooldowns[i] = newCooldown;
            }
            else
            {
                // Ability is now off cooldown.
                m_fCooldowns[i] = 0.0f;
                if (static_cast<eAbility>(i) == ABILITY_ROCKET)
                {
                    m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_ROCKET_RECHARGE, m_bIsPlayer);
                }
            }
        }
    }
    updateTrail(fTimeInSeconds);
    updateSpikes(fTimeInSeconds);
    updateDash(fTimeInSeconds);
}

/*
    Spikes last for some duration after being activated.
    If activated, check if they should be deactivated since last update.
*/
void HovercraftEntity::updateSpikes(float fTimeInSeconds)
{
    if (m_bSpikesActivated)
    {
        m_fSecondsSinceSpikesActivated += fTimeInSeconds;
    }
    if (m_fSecondsSinceSpikesActivated > SPIKES_DURATION)
    {
        m_bSpikesActivated = false;
        m_fSecondsSinceSpikesActivated = 0.0f;
        animateSpikes();
    }
}

/*
    Update the flame indepently over other abilities. Create particles at a
    constant rate if activated, and drain from the gauge.
    Otherwise, recharge.

    @TODO break this up into more functions
*/
void HovercraftEntity::updateTrail(float fTimeInSeconds)
{
    if (m_bTrailActivated)
    {
        m_fSecondsSinceLastFlame += fTimeInSeconds;
        if (m_fTrailGauge > TRAIL_GAUGE_EMPTY)
        {

            float newGaugeValue = m_fTrailGauge - fTimeInSeconds;

            if (newGaugeValue > TRAIL_GAUGE_EMPTY)
                m_fTrailGauge = newGaugeValue;
            else
            {
                m_fTrailGauge = TRAIL_GAUGE_EMPTY;
                deactivateTrail();
            }

            float distanceBetweenFlames = distance(m_vPositionOfLastFlame, m_vPosition);

            // if (m_fSecondsSinceLastFlame > FLAME_INTERVAL)
            if (distanceBetweenFlames >= FLAME_SPACING)
                createTrailInstance();
        }
    }
    else
    {
        m_fSecondsSinceTrailDeactivated += fTimeInSeconds;

        if (m_fSecondsSinceTrailDeactivated > TRAIL_RECHARGE_COOLDOWN
            && m_fTrailGauge < TRAIL_GAUGE_FULL)
        {
            float newGaugeValue = m_fTrailGauge + (fTimeInSeconds * m_fTrailRechargeMultipler);
            if (newGaugeValue < TRAIL_GAUGE_FULL)
            {
                m_fTrailGauge = newGaugeValue;
            }
            else
            {
                m_fTrailGauge = TRAIL_GAUGE_FULL;
            }
        }
    }
}

/*
    Create 1 flame entity at the hovercraft location.
*/
void HovercraftEntity::createTrailInstance()
{
    // Adjust Position to drop flames below HC
    vec3 vAdjustedPosition = m_vPosition;
    vAdjustedPosition.y -= 1.0f;
    // Update the position of the last flame
    m_vPositionOfLastFlame = m_vPosition;
    // m_vPositionOfLastFlame = getPosition();
    m_fSecondsSinceLastFlame = 0.0f;

    mat4 m4TransformMat;
    vec3 vNormal;
    m_pPhysicsComponent->getTransformMatrix(&m4TransformMat);
    vNormal = m4TransformMat[1];
    m_pFireTrail->spawnFlame(&vNormal, &vAdjustedPosition);
}

void HovercraftEntity::updateDash(float fTimeInSeconds)
{
    if (m_iDashCharges < DASH_MAX_CHARGE_COUNT)
    {
        m_fDashRecharge -= fTimeInSeconds;
        if (m_fDashRecharge <= 0)
        {
            m_iDashCharges++;
            /* TODO play charge sound effect */
            m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_DASH_RECHARGE, m_bIsPlayer);
            m_fDashRecharge = m_fDashMaxRecharge;
        }
    }
}

/*
    Use an ability. If the ability if off cooldown, the call with be ignored.

    @param ability  to use
    @return true if ability successfully used
*/
bool HovercraftEntity::useAbility(eAbility ability)
{
    if (!isOffCooldown(ability)) {
        return false;
    }

    switch (ability)
    {
    case ABILITY_ROCKET:
        shootRocket();
        break;
    case ABILITY_SPIKES:
        activateSpikes();
        break;
    case ABILITY_TRAIL_ACTIVATE:
        activateTrail();
        break;
    case ABILITY_TRAIL_DEACTIVATE:
        deactivateTrail();
        break;
    case ABILITY_DASH_BACK:
    case ABILITY_DASH_FORWARD:
    case ABILITY_DASH_LEFT:
    case ABILITY_DASH_RIGHT:
        dash(ability);
        break;
    default:
        return false;
    }
    return true;
}

/*
    Check if an ability is available to use.

    @param ability  to check if on cooldown
    @return true is the ability is off cooldown and can be used.
*/
bool HovercraftEntity::isOffCooldown(eAbility ability) const
{
    /*
    Since there are more abilities than there are cooldown, (each dash shares a
    common cooldown), we must make sure the ability index does not exceed the
    size the m_fCooldowns.

    If the value at that index is greater than 0, then there is still some time
    left to wait before it can be used.
    */
    return m_fCooldowns[ability >= COOLDOWN_COUNT ? COOLDOWN_DASH : ability] <= 0;
}

/*
    Move the hovercraft along the x-y plane (horizontal). Movement commands
    will ignore if the hovercraft is too high off the ground.

    Map view:
                  y = 1
                    ^
                    |
    x = -1 <--- hovercraft ---> x = 1
                    |
                    v
                  y = -1

    @param x    x-coordinate to move
    @param y    y-coordinate to move
*/
void HovercraftEntity::move(float x, float y)
{
    if (lowEnoughToMove && inControl)
    {
        m_pPhysicsComponent->move(x, y);
    }
}

/*
    Turn the hovercraft horizontally.  The larger the direction magnitude, the
    faster the turn in either direction.

    Map view:
                  x = 0
                    ^
                    |
    x = -1 <--- hovercraft ---> x = 1

    @param x    direction to turn
*/
void HovercraftEntity::turn(float x)
{
    if (lowEnoughToMove && inControl)
    {
        m_pPhysicsComponent->rotatePlayer(x);
    }
}

/*
    Shoot a rocket and put it on cool down.
*/
void HovercraftEntity::shootRocket()
{
    mat4 m4CurrentTransform;
    vec3 vVelocity;
    m_pPhysicsComponent->getTransformMatrix(&m4CurrentTransform);
    vVelocity = normalize(m4CurrentTransform[2]);
    
    // The rocket is at the hovercraft's origin
    float translateUp = -0.5f, translateForward = 5.0f; // + is up, - is down
    m4CurrentTransform = translate((vVelocity * translateForward) + vec3(0.0f, translateUp, 0.0f)) * m4CurrentTransform;
    vVelocity *= Rocket::LAUNCH_SPEED;
    m_pRocket->launchRocket(m_iID, &m4CurrentTransform, &vVelocity, Rocket::BOUNDING_BOX, true);
    m_fCooldowns[COOLDOWN_ROCKET] = m_fMaxCooldowns[COOLDOWN_ROCKET];
}

/*
    Activate spikes and put it on cool down.
*/
void HovercraftEntity::activateSpikes()
{
    m_pSoundMngr->play(SoundManager::SOUND_SPIKES_ACTIVATE);

    m_fCooldowns[COOLDOWN_SPIKES] = m_fMaxCooldowns[COOLDOWN_SPIKES];

    m_bSpikesActivated = true;
    m_fSecondsSinceSpikesActivated = 0.0f;
    animateSpikes();
}

/*
    Activate trail and drain from the fuel gauge until it is deactivated.
*/
void HovercraftEntity::activateTrail()
{
    // Trail start sound does always begin, as we cannot guarantee there is
    // any fuel due to trail recharge cooldown. Need to check fuel first.
    if (m_fTrailGauge > TRAIL_GAUGE_EMPTY)
    {
        m_pSoundMngr->startLoop(SoundManager::SOUND_TRAIL, 0, 0);
        m_bTrailActivated = true;
        m_fSecondsSinceLastFlame = 0.0f;
        m_fSecondsSinceTrailDeactivated = 0.0f;
    }

}

/*
    Deactivate the trail and start recharging the fuel gauge.
*/
void HovercraftEntity::deactivateTrail()
{
    if (m_bTrailActivated)
    {
        m_pSoundMngr->endLoop(SoundManager::SOUND_TRAIL, 0, 0);
        m_bTrailActivated = false;
        m_vPositionOfLastFlame = vec3(numeric_limits<float>::max());    // Set Last Position so next spawn will always spawn
    }
}

void HovercraftEntity::dash(eAbility direction)
{
    if (!canDash()) {
        return;
    }

    m_pSoundMngr->play(SoundManager::SOUND_DASH_ACTIVATE);
    switch (direction)
    {
    case ABILITY_DASH_BACK:
        m_pPhysicsComponent->dash(0, -1);
        break;
    case ABILITY_DASH_FORWARD:
        m_pPhysicsComponent->dash(0, 1);
        break;
    case ABILITY_DASH_LEFT:
        m_pPhysicsComponent->dash(-1, 0);
        break;
    case ABILITY_DASH_RIGHT:
        m_pPhysicsComponent->dash(1, 0);
        break;
    }

    m_fCooldowns[COOLDOWN_DASH] = m_fMaxCooldowns[COOLDOWN_DASH];
    m_iDashCharges--;
}

void HovercraftEntity::push(float x, float y) {
        m_pPhysicsComponent->push(x, y);
}

float HovercraftEntity::getTrailGaugePercent() const
{
    return m_fTrailGauge / TRAIL_GAUGE_FULL;
}

void HovercraftEntity::setInvincible()
{
    m_bInvincible = true;
    m_fSecondsLeftUntilVulnerable = INVINCIBLE_TIME;
}

bool HovercraftEntity::isDashing() const
{
    return m_pPhysicsComponent->isDashing();
}

/*
    Reflect a rocket of a specified tranform and direction.
    This will silently launch a new rocket in the opposite direction, and will
    not affect cooldowns or rocket fire count.
*/
void HovercraftEntity::reflectRocket(const mat4 &transform, const vec3 &velocity)
{
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_REFLECT);
    cout << "rocket reflected" << endl;
}

void HovercraftEntity::correspondToEHovercraft(eHovercraft hovercraft)
{
    m_eHovercraft = hovercraft;
    m_bIsPlayer = m_pGameStats->isPlayer(hovercraft);
}

// Activates Spikes
void HovercraftEntity::animateSpikes()
{
    for (unsigned int i = 0; i < NUM_SPIKES; ++i)
        m_pSpikeAnimations[i]->animateToNextFrame();
    m_pSoundMngr->play(SoundManager::eSoundEvent::SOUND_SPIKES_ACTIVATE);
}

