#include "EntityHeaders/HovercraftEntity.h"
#include "EntityHeaders/FlameTrail.h"
#include "EntityHeaders/Rocket.h"
#include "EntityHeaders/Spikes.h"
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
#define ROCKET_SPEED            50.0f
#define FLAME_SPACING           0.25f


#define LOSE_CONTROL_COLLISION_TIME 0.8f
#define LOSE_CONTROL_COLLISION_ELEVATION 2.3f
// Fire Defines
#define FIRE_HEIGHT             2.0
#define FIRE_WIDTH              2.0

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
#define BACK_CAMERA_LONGITUDE   FRONT_CAMERA_LONGITUDE  // theta    degrees

/*
This determines the pitch that the camera
*/
#define BACK_CAMERA_LATITUDE    40.0f   // phi      degrees

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
    m_pGmStats                  = GAME_STATS;

    // Unused?
    // m_fMinimumDistanceBetweenFlames = 5.0f;
    reinitialize();
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
    m_pSpikes->updateWorldTransform(&m4NewTransform);

    // If there's a new Transformation, apply it to the Mesh.
    m_pMesh->updateInstance(&m4NewTransform, m_sName);

    // Check to update Dynamic Position in Spatial Map
    vec3 vNewPosition = m4NewTransform[3];
    if (m_vPosition != vNewPosition)
    {
        m_pSpatialMap->updateDynamicPosition(this, &vNewPosition);
    }

    // Calculate Position Averages for Camera
    m_vPosition = vNewPosition;
    updateInControl(fTimeInSeconds);
    updateCameraLookAts(fTimeInSeconds);
    updateCooldowns(fTimeInSeconds);
    updateVulnerability(fTimeInSeconds);
    updatePowerups(fTimeInSeconds);
}

/*
    Tells the HovercraftEntity that they were damaged. This is where the
    Hovercraft Entity will handle its "death" logic and award points to the
    attacker.

    @param  attacker        to award points to
*/
void HovercraftEntity::getHitBy(eHovercraft attacker)
{
    if (isInvincible()) {
        return;
    }
    setInvincible();
    m_pGmStats->addScore(attacker,
                         static_cast<GameStats::eAddScoreReason>(GAME_STATS->getEHovercraft(m_iID)));
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
        m_vPowerupsEnabled[powerup] -= fTimeInSeconds;
        if (m_vPowerupsEnabled[powerup] <= 0)
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
        break;
    }
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
        break;
    }
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
                                  float fScale)
{
    // Load Mesh and Rendering Component
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pObjectProperties, m_sName, fScale);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, true, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);
    m_vPosition = pObjectProperties->vPosition;

    vec3 vNegCorner, vPosCorner;
    getSpatialDimensions(&vNegCorner, &vPosCorner);
    ObjectInfo::BoundingBox sBounding;
    sBounding.vDimensions = vPosCorner - vNegCorner;

    // PHYSICSTODO: Set up Physics Component as a Dynamic Physics Object for a player
    m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
    m_pPhysicsComponent->initializeVehicle(getName(), true, m_pMesh, &sBounding, pObjectProperties->vPosition);

    // Set up Mesh for Initial Transformation drawing.
    mat4 m4InitialTransform;
    m_pPhysicsComponent->getTransformMatrix(&m4InitialTransform);
    m_pMesh->addInstance(&m4InitialTransform, m_sName);

    // The fire trail entity is always at the same location as the hovecraft
    m_pFireTrail = ENTITY_MANAGER->generateFlameTrailEntity(&m_vPosition, m_iID, FIRE_HEIGHT, FIRE_WIDTH);
    m_pFireTrail->initialize();

    // Create Rocket Mesh
    m_pRocket = SCENE_LOADER->createRocketMesh(m_iID);

    // Create Spikes Mesh
    m_pSpikes = SCENE_LOADER->createSpikesMesh(m_iID);

    // Generate Camera Components
    for (unsigned int i = 0; i < MAX_CAMERAS_PER_PLAYER; ++i)
    {
        m_pCmrComponents[i] = ENTITY_MANAGER->generateCameraComponent(m_iID);
        m_pCmrComponents[i]->setLookAt(m_vPosition);
    }

    m_pCmrComponents[FRONT_CAMERA]->setSphericalPos(FRONT_CAMERA_START_VIEW);
    m_pCmrComponents[BACK_CAMERA]->setSphericalPos(BACK_CAMERA_START_VIEW);
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
    HovercraftEntity* pOtherHovercraft;
    InteractableEntity* pOtherIE;
    switch (eOtherType)
    {
    case ENTITY_HOVERCRAFT:
        // Cast the other Entity to a Hovercraft Entity (We know this is
        // possible because of the two cases)
        pOtherHovercraft = static_cast<HovercraftEntity*>(pOther);
        if (m_bSpikesActivated)
        {   // Tell the Targetted Entity that they were hit by this bot.
           pOtherHovercraft->getHitBy(GAME_STATS->getEHovercraft(m_iID));
        }
        if (pOtherHovercraft->hasSpikesActivated())
        {
            this->getHitBy(GAME_STATS->getEHovercraft(pOtherHovercraft->getID()));
        }

        // Momentarily lose control of vehicle to prevent air moving
        setLoseControl(LOSE_CONTROL_COLLISION_TIME);
        pOtherHovercraft->setLoseControl(LOSE_CONTROL_COLLISION_TIME);
        break;
    case ENTITY_INTERACTABLE:
        // Static Cast to an Interactable Entity
        pOtherIE = static_cast<InteractableEntity*>(pOther);

        // Switch between different Interactable Entity Types
        switch (pOtherIE->getInteractableType())
        {
        case INTER_POWERUP:
            // Random for now
            setPowerup(static_cast<ePowerup>(FuncUtils::random(0, POWERUP_COUNT - 1)));
            break;
        case INTER_FLAME_TRAIL:
        case INTER_ROCKET:
        case INTER_SPIKES:
            this->getHitBy(GAME_STATS->getEHovercraft(pOtherIE->getOwnerID()));
        }
        break;
    case ENTITY_PLANE:
        // TODO still not sure if we're doing the gain control or the elevation check
        // to make collisions less wonky
        // setGainControl();
        break;
        /*Further Cases:
        case ENTITY_ROCKET:
            Get Owner Entity for Rocket (Player or Bot) and call Hit on this Entity to be hit by that player.
        case EXPLOSION:
            Get Owner Entity for Explosion (player or Bot) and call Hit on thie Entity to be hit by that player.
        */
    }
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

    m_fTrailGauge = TRAIL_GAUGE_FULL;
    m_fSecondsSinceLastFlame = 0.0f;
}

/*
    Disables all powerups.
*/
void HovercraftEntity::initializePowerups()
{
    for (int powerup = 0; powerup < POWERUP_COUNT; powerup++)
    {
        m_vPowerupsEnabled[powerup] = 0;
        disablePowerup(static_cast<ePowerup>(powerup));
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
    if (!isInControl)
    {
        outOfControlTime -= fTimeInSeconds;
        if (outOfControlTime <= 0)
        {
            isInControl = true;
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
        float newCooldown = m_fCooldowns[i] - fTimeInSeconds;
        m_fCooldowns[i] = newCooldown > 0.0f ? newCooldown : 0.0f;
    }
    updateTrail(fTimeInSeconds);
    updateSpikes(fTimeInSeconds);
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
            float newGaugeValue = m_fTrailGauge + (fTimeInSeconds * TRAIL_RECHARGE_MULTIPLIER);
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

/*
    Use an ability. If the ability if off cooldown, the call with be ignored.

    @param ability  to use
    @return true if ability successfully used
*/
bool HovercraftEntity::useAbility(eAbility ability)
{
    if (isOnCooldown(ability)) {
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
    Check if an ability is on cooldown.

    @param ability  to check if on cooldown
    @return true is the ability on cooldown and cannot be used.
*/
bool HovercraftEntity::isOnCooldown(eAbility ability)
{
    /*
    Since there are more abilities than there are cooldown, (each dash shares a
    common cooldown), we must make sure the ability index does not exceed the
    size the m_fCooldowns.

    If the value at that index is greater than 0, then there is still some time
    left to wait before it can be used.
    */
    return m_fCooldowns[ability >= COOLDOWN_COUNT ? COOLDOWN_DASH : ability] > 0;
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
    if (lowEnoughToMove && isInControl)
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
    if (lowEnoughToMove && isInControl)
    {
        m_pPhysicsComponent->rotatePlayer(x);
    }
}

/*
    Set a powerup to enable. Once enabled, it provides the powerup benefits for
    a set duration before being disabled and the benefits are lost.

    @param powerup  to enable
*/
void HovercraftEntity::setPowerup(ePowerup powerup)
{
    m_vPowerupsEnabled[powerup] = POWERUP_TIME;
    enablePowerup(powerup);
}

/*
    Shoot a rocket and put it on cool down.
*/
void HovercraftEntity::shootRocket()
{
    mat4 m4CurrentTransform;
    vec3 vVelocity;
    m_pPhysicsComponent->getTransformMatrix(&m4CurrentTransform);
    m_pPhysicsComponent->getDirectionVector(&vVelocity);
    vVelocity *= ROCKET_SPEED;
    m_pRocket->launchRocket(&m4CurrentTransform, &vVelocity, 0.5f);
    m_fCooldowns[COOLDOWN_ROCKET] = ROCKET_COOLDOWN;
}

/*
    Activate spikes and put it on cool down.
*/
void HovercraftEntity::activateSpikes()
{
    SOUND_MANAGER->play(SoundManager::SOUND_SPIKES_ACTIVATE);

    m_fCooldowns[COOLDOWN_SPIKES] = SPIKES_COOLDOWN;

    m_bSpikesActivated = true;
    m_fSecondsSinceSpikesActivated = 0.0f;

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
        SOUND_MANAGER->startLoop(SoundManager::SOUND_TRAIL, 0, 0);
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
    SOUND_MANAGER->endLoop(SoundManager::SOUND_TRAIL, 0, 0);
    m_bTrailActivated = false;
    m_vPositionOfLastFlame = vec3(numeric_limits<float>::max());    // Set Last Position so next spawn will always spawn
}

void HovercraftEntity::dash(eAbility direction)
{
    SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_DASH);
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

    m_fCooldowns[COOLDOWN_DASH] = DASH_COOLDOWN;
}
