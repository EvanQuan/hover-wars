#include "EntityHeaders/HovercraftEntity.h"
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
Cooldowns

The time the hovercraft must wait until they can use the ability again.

Units: seconds
*/
#define ROCKET_COOLDOWN         5.0f
#define SPIKES_COOLDOWN         2.0f
#define TRAIL_COOLDOWN          0.0f
#define DASH_COOLDOWN           5.0f

/*
Total time the trail can be activated from full to empty.

Unit: seconds
*/
#define TRAIL_GAUGE_FULL        3.0f
/*
Represents the trail gauge is empty.
*/
#define TRAIL_GAUGE_EMPTY       0.0f

/*
Total time for the trail to recharge from empty to full.

@NOTE: Currently unused. Right now the rate of drain and
recharge are identical.

Unit: seconds
*/
#define TRAIL_RECHARGE          5.0f

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

@TODO this is potentially not behaving in seconds, like the charge
drain/recharge. Difficult to tell without visual indicators.

Unit: seconds
*/
#define TRAIL_RECHARGE_COOLDOWN 0.1f

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
#define FRONT_CAMERA_LONGITUDE  -90.0f

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
#define BACK_CAMERA_LONGITUDE   -90.0f  // theta    degrees

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
#define BACK_CAMERA_OFFSET      -10     // r        meters

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
const vec3 FRONT_CAMERA_POSITION_OFFSET = vec3(FRONT_CAMERA_OFFSET, 0, 0);
const vec3 BACK_CAMERA_POSITION_OFFSET = vec3(BACK_CAMERA_OFFSET, 0, 0);


HovercraftEntity::HovercraftEntity(int iID, const vec3* vPosition, eEntityTypes entityType)
    : Entity(iID, *vPosition, entityType)
{
    m_pSpatialMap = SPATIAL_DATA_MAP;
    activeCameraIndex = FRONT_CAMERA;
    m_qCurrentCameraRotation = quat();
    m_vCurrentCameraPosition = vec3(0.0f);

    initializeCooldowns();
}

HovercraftEntity::~HovercraftEntity()
{

}

/****************************************************************\
 * Inherited Pure Virtual Functions                             *
\****************************************************************/

/*
@param fSecondsSinceLastUpdate  delta time since last update
*/
void HovercraftEntity::update(float fTimeInMilliseconds)
{
    // New Transformation Matrix
    mat4 m4NewTransform = mat4(1.0f);

    // Get the Transformation from the Physics component
    m_pPhysicsComponent->getTransformMatrix(&m4NewTransform);

    // If there's a new Transformation, apply it to the Mesh.
    m_pMesh->addInstance(&m4NewTransform);
    m_pMesh->addBBInstance(&m4NewTransform);

    // Check to update Dynamic Position in Spatial Map
    vec3 vNewPosition = m4NewTransform[3];
    if (m_vPosition != vNewPosition)
    {
        m_pSpatialMap->updateDynamicPosition(this, &vNewPosition);
    }

    // Calculate Position Averages for Camera
    m_vPosition = vNewPosition;
    updateCameraLookAts(fTimeInMilliseconds);
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
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pObjectProperties, fScale);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, false, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);

    // PHYSICSTODO: Set up Physics Component as a Dynamic Physics Object for a player
    m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
    m_pPhysicsComponent->initializeComponent(true, m_pMesh, &(pObjectProperties->sObjBoundingBox));

    m_pFireTrail = ENTITY_MANAGER->generateInteractableEntity(&m_vPosition);
    m_pFireTrail->loadAsBillboard(FIRE_HEIGHT, FIRE_WIDTH);
    
    // Generate Camera Components
    for (unsigned int i = 0; i < MAX_CAMERAS_PER_PLAYER; ++i)
    {
        m_pCmrComponents[i] = ENTITY_MANAGER->generateCameraComponent(m_iID);
        m_pCmrComponents[i]->setLookAt(m_vPosition);
    }
    
    m_pCmrComponents[FRONT_CAMERA]->setSphericalPos(FRONT_CAMERA_START_VIEW);
    m_pCmrComponents[BACK_CAMERA]->setSphericalPos(BACK_CAMERA_START_VIEW);
}

/********************************************************************************************************\
 * Private Functions                                                                                    *
\********************************************************************************************************/

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
Updates an average for this player's cameras. This is what makes the camera
sway as the player moves.
*/
void HovercraftEntity::updateCameraLookAts(float fSecondsSinceLastUpdate)
{
    updateCameraRotation(fSecondsSinceLastUpdate);
    updateCameraPosition(fSecondsSinceLastUpdate);
    updateCooldowns(fSecondsSinceLastUpdate);
}

void HovercraftEntity::updateCameraRotation(float fSecondsSinceLastUpdate)
{
    quat qCurrRotation = m_pPhysicsComponent->getRotation();
    if (qCurrRotation != m_qCurrentCameraRotation)
    {
        quat cameraRotationDirection = qCurrRotation - m_qCurrentCameraRotation;
        float fSpring = SPRING_ROTATION_CONSTANT * (length(cameraRotationDirection) - CAMERA_REST_LENGTH);

        m_qCurrentCameraRotation += (normalize(cameraRotationDirection) * fSpring) * fSecondsSinceLastUpdate;

        m_pCmrComponents[FRONT_CAMERA]->setRotationQuat(m_qCurrentCameraRotation);
        m_pCmrComponents[BACK_CAMERA]->setRotationQuat(m_qCurrentCameraRotation);
    }
}

void HovercraftEntity::updateCameraPosition(float fSecondsSinceLastUpdate)
{
    vec3 cameraLength = m_vPosition - m_vCurrentCameraPosition;

    if (vec3(0.0f) != cameraLength) // Don't proceed if there's no change.
    {
        float fSpring = SPRING_MOVEMENT_CONSTANT * (length(cameraLength) - CAMERA_REST_LENGTH);
        m_vCurrentCameraPosition += (normalize(cameraLength) * fSpring) * (fSecondsSinceLastUpdate);
      
        // Update all the camera look at and rotation values based on the averaging calculations.
        m_pCmrComponents[FRONT_CAMERA]->setLookAt(m_vCurrentCameraPosition + FRONT_CAMERA_POSITION_OFFSET);
        m_pCmrComponents[BACK_CAMERA]->setLookAt(m_vCurrentCameraPosition + BACK_CAMERA_POSITION_OFFSET);
    }
}

/*
This will decrease the cooldown value all all abilities by the time that has
passed.
*/
void HovercraftEntity::updateCooldowns(float fSecondsSinceLastUpdate)
{
    for (int i = 0; i < ABILITY_COUNT; i++)
    {
        float newCooldown = m_fCooldowns[i] - fSecondsSinceLastUpdate;
        m_fCooldowns[i] = newCooldown > 0.0f ? newCooldown : 0.0f;
    }
    updateTrail(fSecondsSinceLastUpdate);
    // check flame trail separately
}

/*
Update the flame indepently over other abilities. Create particles at a
constant rate if activated, and drain from the gauge.
Otherwise, recharge.

@TODO break this up into more functions
*/
void HovercraftEntity::updateTrail(float fSecondsSinceLastUpdate)
{
    if (m_bTrailActivated)
    {
        if (m_fTrailGauge > TRAIL_GAUGE_EMPTY)
        {
            m_fSecondsSinceLastFlame += fSecondsSinceLastUpdate;
    
            float newGaugeValue = m_fTrailGauge - fSecondsSinceLastUpdate;

            if (newGaugeValue > TRAIL_GAUGE_EMPTY)
            {
                m_fTrailGauge = newGaugeValue;
            }
            else
            {
                m_fTrailGauge = TRAIL_GAUGE_EMPTY;
                deactivateTrail();
            }
            if (m_fSecondsSinceLastFlame > FLAME_INTERVAL)
            {
                createTrailInstance();
    
                m_fSecondsSinceLastFlame = 0.0f;
            }
        }
    }
    else
    {
        m_fSecondsSinceTrailDeactivated += fSecondsSinceLastUpdate;

        if (m_fSecondsSinceTrailDeactivated > TRAIL_RECHARGE_COOLDOWN
            && m_fTrailGauge < TRAIL_GAUGE_FULL)
        {
            float newGaugeValue = m_fTrailGauge + fSecondsSinceLastUpdate;
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
Create 1 flame entity
*/
void HovercraftEntity::createTrailInstance()
{
    mat4 m4TransformMat;
    vec3 vNormal;
    m_pPhysicsComponent->getTransformMatrix(&m4TransformMat);
    vNormal = m4TransformMat[1];
    m_pFireTrail->addBillboard(&vNormal, &m_vPosition);

}

/*
@return true if ability successfully used
*/
bool HovercraftEntity::useAbility(eAbility ability)
{
    if (isOnCooldown(ability))
        return false;

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

void HovercraftEntity::move(float x, float y)
{
    m_pPhysicsComponent->movePlayer(x, y);
}

void HovercraftEntity::turn(float x)
{
    m_pPhysicsComponent->rotatePlayer(x);
}

/*
Shoot a rocket and put it on cool down.
*/
void HovercraftEntity::shootRocket()
{
    SOUND_MANAGER->playEvent(SoundManager::SOUND_ROCKET_ACTIVATE);

    EMITTER_ENGINE->generateEmitter(m_vPosition, vec3(0, 1, 0), 60.f, 5.0f, 5, false, 2.0f);

    m_fCooldowns[COOLDOWN_ROCKET] = ROCKET_COOLDOWN;
}

/*
Activate spikes and put it on cool down.
*/
void HovercraftEntity::activateSpikes()
{
    SOUND_MANAGER->playEvent(SoundManager::SOUND_SPIKES_ACTIVATE);

    m_fCooldowns[COOLDOWN_SPIKES] = SPIKES_COOLDOWN;

}

/*
Activate trail and drain from the fuel gauge until it is deactivated.
*/
void HovercraftEntity::activateTrail()
{
    /*
    Later, this should be split into starting and ending the trail event loop
    */
    // SOUND_MANAGER->playEvent(SoundManager::SOUND_TRAIL);
    SOUND_MANAGER->startLoop(SoundManager::SOUND_TRAIL, 0, 0);

    m_bTrailActivated = true;
    m_fSecondsSinceLastFlame = 0.0f;
    m_fSecondsSinceTrailDeactivated = 0.0f;
}

/*
Deactivate the trail and start recharging the fuel gauge.
*/
void HovercraftEntity::deactivateTrail()
{
    SOUND_MANAGER->endLoop(SoundManager::SOUND_TRAIL, 0, 0);
    m_bTrailActivated = false;
}

void HovercraftEntity::dash(eAbility direction)
{
    SOUND_MANAGER->playEvent(SoundManager::SOUND_HOVERCAR_DASH);
    switch (direction)
    {
    case ABILITY_DASH_BACK:
        break;
    case ABILITY_DASH_FORWARD:
        break;
    case ABILITY_DASH_LEFT:
        break;
    case ABILITY_DASH_RIGHT:
        break;
    }

    m_fCooldowns[COOLDOWN_DASH] = DASH_COOLDOWN;
}
