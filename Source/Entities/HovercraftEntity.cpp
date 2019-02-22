#include "EntityHeaders/HovercraftEntity.h"
#include "MeshManager.h"
#include "EntityManager.h"
#include "SoundManager.h"

HovercraftEntity::HovercraftEntity(int iID, const vec3* vPosition, eEntityTypes entityType)
    : Entity(iID, *vPosition, entityType)
{
    m_pSpatialMap = SPATIAL_DATA_MAP;
    activeCameraIndex = FRONT_CAMERA;

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
    m_pPhysicsComponent->initializeComponent(true, m_pMesh);

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
    quat cameraRotationDirection = m_pPhysicsComponent->getRotation() - m_qCurrentCameraRotation;

    m_qCurrentCameraRotation += cameraRotationDirection * CAMERA_ROTATION_MULTIPLIER * fSecondsSinceLastUpdate;

    m_pCmrComponents[FRONT_CAMERA]->setRotationQuat(m_qCurrentCameraRotation);
    m_pCmrComponents[BACK_CAMERA]->setRotationQuat(m_qCurrentCameraRotation);
}

void HovercraftEntity::updateCameraPosition(float fSecondsSinceLastUpdate)
{
    vec3 cameraMovementDirection = m_vPosition - m_vCurrentCameraPosition;

    m_vCurrentCameraPosition += cameraMovementDirection * CAMERA_MOVEMENT_MULTIPLIER * fSecondsSinceLastUpdate;

    // Update all the camera look at and rotation values based on the averaging calculations.
    m_pCmrComponents[FRONT_CAMERA]->setLookAt(m_vCurrentCameraPosition + m_qCurrentCameraRotation * FRONT_CAMERA_POSITION_OFFSET);
    m_pCmrComponents[BACK_CAMERA]->setLookAt(m_vCurrentCameraPosition + m_qCurrentCameraRotation * BACK_CAMERA_POSITION_OFFSET);

}

/*
This will decrease the cooldown value all all abilities by the time that has
passed.
*/
void HovercraftEntity::updateCooldowns(float fSecondsSinceLastUpdate)
{
    for (int i = 0; i < ABILITY_COUNT; i++)
    {
        if (m_fCooldowns[i] - fSecondsSinceLastUpdate <= 0.0f)
        {
            m_fCooldowns[i] = 0.0f;
        }
        else
        {
            m_fCooldowns[i] -= fSecondsSinceLastUpdate;
        }
    }
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
    case ABILITY_TRAIL:
        activateTrail();
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
    return m_fCooldowns[ability] > 0;
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
    EMITTER_ENGINE->generateEmitter(m_vPosition, vec3(0, 1, 0), 60.f, 5.0f, 5, false, 2.0f);
    SOUND_MANAGER->play(SoundManager::SOUND_ROCKET_ACTIVATE);

    m_fCooldowns[COOLDOWN_ROCKET] = ROCKET_COOLDOWN;
}

/*
Activate spikes and put it on cool down.
*/
void HovercraftEntity::activateSpikes()
{
    GAME_STATS->addScore(PLAYER_1, GameStats::HIT_BOT);
    SOUND_MANAGER->play(SoundManager::SOUND_SPIKES_ACTIVATE);

    m_fCooldowns[COOLDOWN_SPIKES] = SPIKES_COOLDOWN;
}

/*
Activate trail and drain from the fuel gauge until it is deactivated.
*/
void HovercraftEntity::activateTrail()
{
    mat4 m4TransformMat;
    vec3 vNormal;
    m_pPhysicsComponent->getTransformMatrix(&m4TransformMat);
    vNormal = m4TransformMat[1];
    m_pFireTrail->addBillboard(&vNormal, &m_vPosition);

    // May not have a cooldown. May work on a fuel system instead.
    // Will need to implement.
}

/*
Deactivate the trail and start recharging the fuel gauge.
*/
void HovercraftEntity::deactivateTrail()
{
}

void HovercraftEntity::dash(eAbility direction)
{
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
}
