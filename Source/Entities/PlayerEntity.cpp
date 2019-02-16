#include "EntityHeaders/PlayerEntity.h"
#include "MeshManager.h"
#include "EntityManager.h"

/***********\
 * Defines *
\***********/
// Fire Defines
#define FIRE_HEIGHT 2.0
#define FIRE_WIDTH 2.0

// Camera Defines
#define FRONT_CAMERA 0
#define BACK_CAMERA 1
#define PAST_CAMERA_POSITIONS 150
#define AVERAGE_MULTIPLIER (1.0f / static_cast<float>(PAST_CAMERA_POSITIONS))
#define START_RADIUS 10.0f
/*

*/
#define CAMERA_THETA 90.0f
#define CAMERA_PHI 60.0f

const vec3 FRONT_CAMERA_START_VIEW = vec3(-CAMERA_THETA, CAMERA_PHI, START_RADIUS); // (Theta, Phi, Radius)
const vec3 BACK_CAMERA_START_VIEW = vec3(CAMERA_THETA, CAMERA_PHI, START_RADIUS); // (Theta, Phi, Radius)
/*
The position of the camera relative to the position of the player. Both vectors
will be added together to form the final camera position.
*/
const vec3 FRONT_CAMERA_POSITION_OFFSET = vec3(-5, 0, 0);
const vec3 BACK_CAMERA_POSITION_OFFSET = vec3(5, 0, 0);

PlayerEntity::PlayerEntity(int iID, const vec3* vPosition)
    : Entity(iID, *vPosition)
{
    activeCameraIndex = FRONT_CAMERA;
    m_vPositionTotal = *vPosition * PAST_CAMERA_POSITIONS;
    for (unsigned int i = 0; i < PAST_CAMERA_POSITIONS; ++i)
    {
        m_vPastPositions.push(*vPosition);
    }
}

PlayerEntity::~PlayerEntity()
{

}

/****************************************************************\
 * Inherited Pure Virtual Functions                                *
\****************************************************************/

void PlayerEntity::update(float fTimeInMilliseconds)
{
    // New Transformation Matrix
    mat4 m4NewTransform = mat4(1.0f);

    // Get the Transformation from the Physics component
    m_pPhysicsComponent->getTransformMatrix(&m4NewTransform);

    // If there's a new Transformation, apply it to the Mesh.
    m_pMesh->addInstance(&m4NewTransform);
    m_pMesh->addBBInstance(&m4NewTransform);

    // Calculate Position Averages for Camera
    m_vPosition = m4NewTransform[3];
    updateCameraLookAts(); // TODO: Need to interpolate positions a bit better.
}

// Initializes Player Entity information
void PlayerEntity::initializePlayer(const string& sFileName,
                                    const Material* pMaterial,
                                    const string& sShaderType,
                                    float fScale,
                                    const BoundingBox* pBoundingBox)
{
    // Load Mesh and Rendering Component
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pMaterial, pBoundingBox, fScale, m_vPosition);
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

// Updates an Average for this player's cameras.
void PlayerEntity::updateCameraLookAts()
{
    // Queue new position and add to total
    m_vPastPositions.push(m_vPosition);
    m_vPositionTotal += m_vPosition;

    // Keep Queue within limits of Average
    if (m_vPastPositions.size() > PAST_CAMERA_POSITIONS)
    {
        m_vPositionTotal -= m_vPastPositions.front();
        m_vPastPositions.pop();
    }

    // Calculate Average Position and set new look at for Camera Components
    vec3 vAveragedPos = m_vPositionTotal * AVERAGE_MULTIPLIER;

    // Update all the camera look at and rotation values based on the averaging calculations.
    quat rotation = m_pPhysicsComponent->getRotation();
    m_pCmrComponents[FRONT_CAMERA]->setLookAt(vAveragedPos + rotation * FRONT_CAMERA_POSITION_OFFSET);
    m_pCmrComponents[FRONT_CAMERA]->setRotationQuat(rotation);
    m_pCmrComponents[BACK_CAMERA]->setLookAt(vAveragedPos + rotation * BACK_CAMERA_POSITION_OFFSET);
    m_pCmrComponents[BACK_CAMERA]->setRotationQuat(rotation);
}

void PlayerEntity::useAbility(eAbility ability)
{
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
    }
}

void PlayerEntity::move(float x, float y)
{
    PHYSICS_MANAGER->movePlayer(m_iID, x, y);
}

void PlayerEntity::turn(float x)
{
    PHYSICS_MANAGER->rotatePlayer(m_iID, x);
}

void PlayerEntity::shootRocket()
{
    EMITTER_ENGINE->generateEmitter(m_vPosition, vec3(0, 1, 0), 60.f, 5.0f, 100, false, 2.0f);
}

void PlayerEntity::activateSpikes()
{

}

void PlayerEntity::activateTrail()
{
    mat4 m4TransformMat;
    vec3 vNormal;
    m_pPhysicsComponent->getTransformMatrix(&m4TransformMat);
    vNormal = m4TransformMat[1];
    m_pFireTrail->addBillboard(&vNormal, &m_vPosition);
}

void PlayerEntity::dash(eAbility direction)
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
