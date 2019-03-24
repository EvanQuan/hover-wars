#include "ArtificialIntelligence/AIManager.h"
#include "EntityManager.h"
#include "EntityComponentHeaders/PhysicsComponent.h"


AIManager* AIManager::m_pInstance = nullptr;

AIManager::AIManager()
{

}

AIManager::~AIManager()
{
    m_vAIComponents.clear();
}

AIManager* AIManager::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new AIManager();
    }
    return m_pInstance;
}

/*
    Initialize everything that's needed for the start of a new game
*/
void AIManager::reinitialize()
{
    m_vAIComponents.clear();
    const vector<HovercraftEntity*>* bots = ENTITY_MANAGER->getBotList();
    for (size_t i = 0, size = bots->size(); i < size; i++)
    {
        HovercraftEntity* bot = bots->at(i);
        AIComponent* ai = ENTITY_MANAGER->generateAIComponent(bot->getID());
        initializeAIComponent(bot, ai);
        m_vAIComponents.push_back(ai);
    }
}

/*
    Initialize a specified hovercraft with an AIComponent.
*/
void AIManager::initializeAIComponent(HovercraftEntity* bot, AIComponent* ai)
{
    glm::vec3 botVel = bot->m_pPhysicsComponent->getLinearVelocity();
    glm::vec3 botPos = bot->m_pPhysicsComponent->getPosition();
    PxTransform globalTransform = bot->m_pPhysicsComponent->getGlobalPose();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = ENTITY_MANAGER->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->getPosition();
    glm::vec3 playerVel = ENTITY_MANAGER->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->m_pPhysicsComponent->getLinearVelocity();
    ai->initalize(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z));
}

/*
    Update all the AI for a given time frame.
    This should be called every frame in game, but should not be updated out of
    game (such as when the game is paused, or the players are in other menus).
*/
void AIManager::update(float fTimeInSeconds)
{
    const vector<HovercraftEntity*>* bots = ENTITY_MANAGER->getBotList();
    for (size_t i = 0, size = m_vAIComponents.size(); i < size; ++i)
    {
        AIComponent* ai = m_vAIComponents.at(i);
        HovercraftEntity* bot = bots->at(i);
        PhysicsComponent* physics = bot->m_pPhysicsComponent;

        //vector<vec2> path = SPATIAL_DATA_MAP->aStarSearch(vec2(18,21),vec2(18,19));
        //std::cout << "path size: " << path.size() << std::endl;
        //unsigned int minX, minY, maxX, maxY;
        //SPATIAL_DATA_MAP->getMapIndices(this,&minX,&minY,&maxX,&maxY);
        //std::cout << "mapIndices: " << minX << "," << minY << std::endl;
        ai->update(fTimeInSeconds);
        glm::vec3 botVel = physics->getLinearVelocity();
        glm::vec3 botPos = physics->getPosition();
        glm::quat rotation = physics->getRotation();
        // double x = FuncUtils::getRoll(rotation);
        // double y = FuncUtils::getPitch(rotation);
        // double z = FuncUtils::getYaw(rotation);

        PxTransform globalTransform = physics->getGlobalPose();
        PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));

        int playerNum = eHovercraft::HOVERCRAFT_PLAYER_1;
        float distanceToNearestPlayer = 1000000;
        // @TODO add ability to change targets when we do multiplayer
        /*for (int i = 0; i < ENTITY_MANAGER->getPlayerSize(); i++) {
            HovercraftEntity *mPlayer = ENTITY_MANAGER->getPlayer((eHovercraft)i);
            float dis = (mPlayer->getPosition() - botPos).length();
            if ((mPlayer->getPosition() - botPos).length() < distanceToNearestPlayer) {
                distanceToNearestPlayer = dis;
                playerNum = i;
            }
        }*/
        HovercraftEntity *mPlayer = ENTITY_MANAGER->getPlayer((eHovercraft)playerNum);
        //glm::vec3 targetPosition = mPlayer->getPosition();


        glm::vec3 targetVelocity = mPlayer->m_pPhysicsComponent->getLinearVelocity();


        //vector<vec2> path = SPATIAL_DATA_MAP->aStarSearch(vec2(minXPlayer, minYPlayer),vec2(minXBot, minYBot));
        Action a;
        ai->popCurrentAction(mPlayer, bot,targetVelocity, botPos, botVel, atan2(vForce.x, vForce.z),&a);

        //std::cout << "BotEntity update: " << a.actionsToTake[0] << ", " << a.actionsToTake[1] << ", " << a.actionsToTake[2] << ", "<< a.actionsToTake[3] << std::endl;
        // fire Rocket, right-left turn, forward-back move,right-left move
        //std::cout << vForce.x <<  "x: " << vForce.y << " y: " << sin(vForce.z) << std::endl;

        float turnValue = a.actionsToTake[AIComponent::eAction::ACTION_TURN];
        if (turnValue != 0) {
            bot->turn(turnValue);
        }
        float moveX = a.actionsToTake[AIComponent::eAction::ACTION_MOVE_RIGHT_LEFT];
        float moveY = a.actionsToTake[AIComponent::eAction::ACTION_MOVE_FORWARDS_BACKWARDS];
        if (((int)moveX != 0) || ((int)moveY != 0)) {
            if (bot->isInControl()) {
                physics->moveGlobal(moveX, moveY);
            }
        }
        if (a.actionsToTake[AIComponent::eAction::ACTION_FIRE_ROCKET] == 1) {
            bot->useAbility(eAbility::ABILITY_ROCKET);
        }
        bot->update(fTimeInSeconds);
    }
  
}
