#include "EntityHeaders/PlayerEntity.h"
#include "MeshManager.h"
#include "EntityManager.h"

/***********\
 * Defines *
\***********/
#define FRONT_CAMERA 0
#define BACK_CAMERA 1
#define MAX_NUM_POS 30
#define AVERAGE_MULTIPLIER (1.0f / static_cast<float>(MAX_NUM_POS))
#define START_RADIUS 10.0f
const vec3 FRONT_CAMERA_START_POS = vec3(60.f, 20.f, START_RADIUS); // (Theta, Phi, Radius)
const vec3 BACK_CAMERA_START_POS = vec3(-60.f, 20.f, START_RADIUS); // (Theta, Phi, Radius)

PlayerEntity::PlayerEntity(int iID, const vec3* vPosition)
    : Entity(iID, *vPosition)
{
    m_vPositionTotal = *vPosition * MAX_NUM_POS;
    for (unsigned int i = 0; i < MAX_NUM_POS; ++i) m_vPastPositions.push(*vPosition);
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

    // Calculate Position Averages for Camera
    m_vPosition = m4NewTransform[3];
    //updateCameraLookAts(); // TODO: Need to interpolate positions a bit better.
}

// Initializes Player Entity information
void PlayerEntity::initializePlayer(const string& sFileName,
                                    const Material* pMaterial,
                                    const string& sShaderType,
                                    float fScale)
{
    // Load Mesh and Rendering Component
    m_pMesh = MESH_MANAGER->loadMeshFromFile(sFileName, pMaterial, fScale, m_vPosition);
    m_pRenderComponent = ENTITY_MANAGER->generateRenderComponent(m_iID, m_pMesh, false, SHADER_MANAGER->getShaderType(sShaderType), GL_TRIANGLES);

    // PHYSICSTODO: Set up Physics Component as a Dynamic Physics Object for a player
    m_pPhysicsComponent = ENTITY_MANAGER->generatePhysicsComponent(m_iID);
    m_pPhysicsComponent->initializeComponent(true, m_pMesh);
    
    // Generate Camera Components
    //for (unsigned int i = 0; i < MAX_CAMERAS_PER_PLAYER; ++i)
    //{
    //  m_pCmrComponents[i] = ENTITY_MANAGER->generateCameraComponent(m_iID);
    //  m_pCmrComponents[i]->setLookAt(m_vPosition);
    //}
    //
    //m_pCmrComponents[FRONT_CAMERA]->setSphericalPos(FRONT_CAMERA_START_POS);
    //m_pCmrComponents[BACK_CAMERA]->setSphericalPos(BACK_CAMERA_START_POS);
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
    if (m_vPastPositions.size() > MAX_NUM_POS)
    {
        m_vPositionTotal -= m_vPastPositions.front();
        m_vPastPositions.pop();
    }

    // Calculate Average Position and set new look at for Camera Components
    vec3 vAveragedPos = m_vPositionTotal * AVERAGE_MULTIPLIER;
    for (unsigned int i = 0; i < MAX_CAMERAS_PER_PLAYER; ++i)
        m_pCmrComponents[i]->setLookAt(vAveragedPos);
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

void PlayerEntity::shootRocket()
{
    EMITTER_ENGINE->generateEmitter(m_vPosition, vec3(0, 1, 0), 60.f, 5.0f, 100, false, 2.0f);
}

void PlayerEntity::activateSpikes()
{

}

void PlayerEntity::activateTrail()
{

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
