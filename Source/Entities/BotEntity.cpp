#include "EntityHeaders/BotEntity.h"
#include "EntityManager.h"
#include "Enums/eCooldown.h"


BotEntity::BotEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition, BOT_ENTITY)
{

}

BotEntity::~BotEntity()
{

}
#define M_PI 3.1415926535
void BotEntity::toEulerAngle(glm::quat q, double& roll, double& pitch, double& yaw)
{
    // roll (x-axis rotation)
    double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    roll = atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = +2.0 * (q.w * q.y - q.z * q.x);
    if (fabs(sinp) >= 1)
        pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        pitch = asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    yaw = atan2(siny_cosp, cosy_cosp);
}

void BotEntity::update(float fTimeInMilliseconds)
{
    
    //m_AIComponent->update(fTimeInMilliseconds);
    glm::vec3 botVel = m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = m_pPhysicsComponent->getPosition();
    double x, y, z;
    toEulerAngle(m_pPhysicsComponent->getRotation(),x,y,z);
    PxTransform globalTransform = m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
    Action a;
    float *coolDowns = getCooldowns();
    m_AIComponent->popCurrentAction(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z), coolDowns[eCooldown::COOLDOWN_ROCKET],&a);
    if (a.actionsToTake[4] == 1) {
        useAbility(eAbility::ABILITY_SPIKES);
    }
    //std::cout << "BotEntity update: " << a.actionsToTake[0] << ", " << a.actionsToTake[1] << ", " << a.actionsToTake[2] << ", "<< a.actionsToTake[3] << std::endl;
    // fire Rocket, right-left turn, forward-back move,right-left move
    //std::cout << vForce.x <<  "x: " << vForce.y << " y: " << sin(vForce.z) << std::endl;
    if (a.actionsToTake[1] != 0) {
        turn(a.actionsToTake[1]);
    }
    if (a.actionsToTake[2] != 0 || a.actionsToTake[3] != 0) {
        m_pPhysicsComponent->moveGlobal(a.actionsToTake[3],a.actionsToTake[2]);
    }
    HovercraftEntity::update(fTimeInMilliseconds);
  
}
void BotEntity::initialize(const string& sFileName,
    const ObjectInfo* pObjectProperties,
    const string& sShaderType,
    float fScale,
    eBot botID)
{
    std::cout << "Bot Entity created" << std::endl;
    HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
    m_eBotID = botID;
    m_AIComponent = ENTITY_MANAGER->generateAIComponent(m_iID);

    glm::vec3 botVel = m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = m_pPhysicsComponent->getPosition();
    PxTransform globalTransform = m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
    m_AIComponent->initalize(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z));
}
