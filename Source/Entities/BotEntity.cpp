#include "EntityHeaders/BotEntity.h"
#include "EntityManager.h"
#include "Enums/eCooldown.h"


BotEntity::BotEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition)
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
    
    m_AIComponent->update(fTimeInMilliseconds);
    glm::vec3 botVel = m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = m_pPhysicsComponent->getPosition();
    double x, y, z;
    toEulerAngle(m_pPhysicsComponent->getRotation(),x,y,z);
    PxTransform globalTransform = m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
    Action a;
    m_AIComponent->popCurrentAction(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z), 0.0f,&a);

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
    unsigned int iStatsID)
{
    HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
    m_AIComponent = ENTITY_MANAGER->generateAIComponent(m_iID);

    glm::vec3 botVel = m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = m_pPhysicsComponent->getPosition();
    PxTransform globalTransform = m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(ePlayer::PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
    m_AIComponent->initalize(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z));
}

/*
    Tells the HovercraftEntity that they were damaged. This is where the Hovercraft Entity will handle its "death" logic and award points to the winner.

    @param  eHitByType      The Entity Type that this Entity was hit by. This entity will either be a bot or a Player

    @TODO   This seems very general and may be able to be reworked with a better design.
*/
void BotEntity::hit(eEntityTypes eHitByType, unsigned int iNumber)
{
    // cout << "Bot " << iNumber << " hit by " << eHitByType << endl;
    // Get Score reason (The Other Entity hit this bot) /*Offset the Bot ID with the Add score offsets*/
    GameStats::eAddScoreReason eScoreReason = static_cast<GameStats::eAddScoreReason>(m_iStatsID + GameStats::eAddScoreReason::HIT_BOT_1 - 1);

    // Switch based on who hit the player
    switch (eHitByType)
    {   // TODO: Fix these Cases to handle one entity type
    case HOVERCRAFT_ENTITY:    // Hitting Entity was a bot, meaning that the bot #iNumber should get points for hitting this player #m_ePlayerID
        if (!isInvincible())
        {
            m_pGmStats->addScore(static_cast<eBot>(iNumber), eScoreReason);
        }
        setInvincible();
        break;
    //case PLAYER_ENTITY: // Hitting Entity was another player, meaning that the player #iNumber should get points for hitting this player #m_ePlayerID
    //    if (!isInvincible())
    //    {
    //        m_pGmStats->addScore(static_cast<ePlayer>(iNumber), eScoreReason);
    //    }
    //    setInvincible();
    //    break;
    }
 
}

