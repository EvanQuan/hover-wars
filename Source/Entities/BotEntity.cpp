#include "EntityHeaders/BotEntity.h"
#include "EntityManager.h"
#include "Enums/eCooldown.h"


/*
@Deprecated This will be deleted once all AI related calculated are moved
outside of this class.
*/
BotEntity::BotEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition)
{

}

BotEntity::~BotEntity()
{

}

void BotEntity::update(float fTimeInSeconds)
{
    //vector<vec2> path = SPATIAL_DATA_MAP->aStarSearch(vec2(18,21),vec2(18,19));
    //std::cout << "path size: " << path.size() << std::endl;
    //unsigned int minX, minY, maxX, maxY;
    //SPATIAL_DATA_MAP->getMapIndices(this,&minX,&minY,&maxX,&maxY);
    //std::cout << "mapIndices: " << minX << "," << minY << std::endl;
    m_AIComponent->update(fTimeInSeconds);
    glm::vec3 botVel = m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = m_pPhysicsComponent->getPosition();
    glm::quat rotation = m_pPhysicsComponent->getRotation();
    // double x = FuncUtils::getRoll(rotation);
    // double y = FuncUtils::getPitch(rotation);
    // double z = FuncUtils::getYaw(rotation);

    PxTransform globalTransform = m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
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
    HovercraftEntity::update(fTimeInSeconds);
  
}
void BotEntity::initialize(const string& sFileName,
    const ObjectInfo* pObjectProperties,
    const string& sShaderType,
    float fScale)
{
    HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);

    m_AIComponent = ENTITY_MANAGER->generateAIComponent(m_iID);

    glm::vec3 botVel = m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = m_pPhysicsComponent->getPosition();
    PxTransform globalTransform = m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
    m_AIComponent->initalize(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z));
}
