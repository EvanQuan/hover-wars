#include "ArtificialIntelligence/AIManager.h"
#include "EntityManager.h"
#include "EntityComponentHeaders/PhysicsComponent.h"


AIManager* AIManager::m_pInstance = nullptr;

AIManager::AIManager()
{
    m_pEntityMngr = ENTITY_MANAGER;
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

void AIManager::reinitialize()
{
    m_vAIComponents.clear();
    const vector<HovercraftEntity*>* bots = m_pEntityMngr->getBotList();
    for (size_t i = 0, size = bots->size(); i < size; i++)
    {
        HovercraftEntity* bot = bots->at(i);
        AIComponent* ai = m_pEntityMngr->generateAIComponent(bot->getID());
        initializeAIComponent(bot, ai);
        m_vAIComponents.push_back(ai);
    }
}

/*
    Initialize a specified hovercraft with an AIComponent.
*/
void AIManager::initializeAIComponent(HovercraftEntity* bot, AIComponent* ai)
{
    glm::vec3 botVel = bot->getLinearVelocity();
    glm::vec3 botPos = bot->getPosition();
    PxTransform globalTransform = bot->getGlobalTransform();
    PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));
    glm::vec3 playerPos = m_pEntityMngr->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->getPosition();
    glm::vec3 playerVel = m_pEntityMngr->getPlayer(eHovercraft::HOVERCRAFT_PLAYER_1)->getLinearVelocity();
    ai->initalize(playerPos, playerVel, botPos, botVel, atan2(vForce.x, vForce.z));
}

/*
    Given the bot position, get the nearest player and the distance between the
    player and the bot.

    @param bot                  to check for the nearest player
    @param botPosition          of bot
    @return nearestPlayer       from the specified botPosition
    @return distanceToPlayer    between botPosition and the nearestPlayer position
*/
void AIManager::getNearestPlayer(const eHovercraft &bot,
                                 const vec3 &botPosition,
                                 eHovercraft &nearestPlayer,
                                 float &distanceToPlayer)
{
    getNearestHovercraft(m_pEntityMngr->getPlayerList(), bot, botPosition, nearestPlayer, distanceToPlayer);
}

/*
    Given the bot position, get the nearest bot and the distance between the
    two bots

    @param bot                  to check for the nearest bot
    @param botPosition          of bot
    @return nearestBot          from the specified botPosition
    @return distanceToBot       between botPosition and the nearestBot position
*/
void AIManager::getNearestBot(const eHovercraft &bot,
                              const vec3 &botPosition,
                              eHovercraft &nearestBot,
                              float &distanceToBot)
{
    getNearestHovercraft(m_pEntityMngr->getBotList(), bot, botPosition, nearestBot, distanceToBot);
}

/*
    Given the bot position, get the nearest hovercraft and the distance between the
    two hovercrafts, choosing from the specified 

    @param hovercraftList           of hovercrafts to check
    @param bot                      to check for the nearest hovercraft
    @param botPosition              of bot
    @return nearestHovercraft       from the specified botPosition
    @return distanceToHovercraft    between botPosition and the nearestHovercraft position
*/
void AIManager::getNearestHovercraft(const vector<HovercraftEntity*>* hovercraftList,
                                     const eHovercraft &bot,
                                     const vec3 &botPosition,
                                     eHovercraft & nearestHovercraft,
                                     float & distanceToHovercraft)
{
    distanceToHovercraft = numeric_limits<float>::max();
    for (int h = 0, size = hovercraftList->size(); h < size; h++) {
        HovercraftEntity *hovercraftEntity = hovercraftList->at(h);

        // Make sure that the bot does not check the distance between itself
        if (bot != hovercraftEntity->getEHovercraft())
        {
            float distance = glm::distance(botPosition, hovercraftEntity->getPosition());
            if (distance < distanceToHovercraft) {
                distanceToHovercraft = distance;
                nearestHovercraft = hovercraftEntity->getEHovercraft();
            }
        }
    }
}

eHovercraft AIManager::getTargetID(eHovercraft nearestPlayer, float distanceToPlayer,
                                   eHovercraft nearestBot, float distanceToBot)
{
    if (nearestBot == HOVERCRAFT_INVALID) {
        // If there are no other bots, then the only choice is the player
        return nearestPlayer;
    }
    // Otherwise, choose the closer one
    return distanceToPlayer < distanceToBot ? nearestPlayer : nearestBot;
}

/*
    Get the hovercraft that this bot should target.

    @param bot                  to find the target
    @param botPosition          of the bot to find the target
    @return target              that the bot should target
*/
HovercraftEntity* AIManager::getTarget(const eHovercraft &bot,
                          const vec3 &botPostion)
{
    eHovercraft nearestPlayer = HOVERCRAFT_PLAYER_1;
    float distanceToPlayer = numeric_limits<float>::max();
    getNearestPlayer(bot, botPostion, nearestPlayer, distanceToPlayer);

    eHovercraft nearestBot = HOVERCRAFT_INVALID;
    float distanceToBot = numeric_limits<float>::max();
    getNearestBot(bot, botPostion, nearestBot, distanceToBot);

    eHovercraft target = getTargetID(nearestPlayer, distanceToPlayer, nearestBot, distanceToBot);

    //cout << "Bot " << bot << " target:" << endl
        //<< "\tnearest player: " << nearestPlayer << " @ " << distanceToPlayer << endl
        //<< "\tnearest bot: " << nearestBot << " @ " << distanceToBot << endl
        //<< "\ttarget: " << target << endl;

    return m_pEntityMngr->getHovercraft(target);
}

/*
    Make a specified bot execute a specified action

    @param bot      to execute action
    @param action   to execute
*/
void AIManager::executeAction(HovercraftEntity *bot, Action &a)
{
    float turnValue = a.turn;
    if (turnValue != 0) {
        bot->turn(turnValue);
    }
    float moveX = a.moveX;
    float moveY = a.moveY;
    if (shouldMove(bot, a)) {
        bot->move(moveX, moveY);
    }
    if (a.shouldFireRocket) {
        bot->useAbility(eAbility::ABILITY_ROCKET);
    }
    if (a.shouldActivateTrail) {
        if (bot->getTrailGaugePercent() > 0.5f) {
            // Nested, so that the bot neither activates or deactivates
            // with a low fuel gauge to prevent audio spam
            bot->useAbility(eAbility::ABILITY_TRAIL_ACTIVATE);
        }
    } else {
        bot->useAbility(eAbility::ABILITY_TRAIL_DEACTIVATE);
    }
}

/*
    Determine whether a bot should move. This prevents sending unecessary
    move commands every update.

    @parm bot   to check if it should move
    @param a    to determine whether the bot should move
*/
bool AIManager::shouldMove(HovercraftEntity *bot, Action &a)
{
    return bot->isInControl()
        && !(static_cast<int>(a.moveX) == 0 && static_cast<int>(a.moveY) == 0);
}

/*
    Update all the AI for a given time frame.
    This should be called every frame in game, but should not be updated out of
    game (such as when the game is paused, or the players are in other menus).
*/
void AIManager::update(float fTimeInSeconds)
{
    const vector<HovercraftEntity*>* bots = m_pEntityMngr->getBotList();
    for (size_t i = 0, size = m_vAIComponents.size(); i < size; ++i)
    {
        AIComponent* ai = m_vAIComponents.at(i);
        HovercraftEntity* bot = bots->at(i);
        eHovercraft botHovercraft = bot->getEHovercraft();

        //vector<vec2> path = SPATIAL_DATA_MAP->aStarSearch(vec2(18,21),vec2(18,19));
        //std::cout << "path size: " << path.size() << std::endl;
        //unsigned int minX, minY, maxX, maxY;
        //SPATIAL_DATA_MAP->getMapIndices(this,&minX,&minY,&maxX,&maxY);
        //std::cout << "mapIndices: " << minX << "," << minY << std::endl;
        ai->update(fTimeInSeconds);
        glm::vec3 botPos = bot->getPosition();

        // @Austin what is going on here? Why are we rotating?
        // PxTransform globalTransform = bot->getGlobalTransform();
        // PxVec3 vForce = globalTransform.q.rotate(PxVec3(0, 1, 0));

        // @TODO Maybe we can the bot not choose a new target every frame, but
        // at a larger interval.
        // The reason for this would be that if a bot is chasing down the player,
        // and some other bot momentarily runs by, then the two bots will
        // instantly lock onto each other until another hovercraft comes closer
        // between them.
        // This results in bots "sticking together" like magnets, which results
        // in some weird behaviour.
        // In addition to this, maybe there should be a chance to pick a random
        // target as well to combat this "magnetic" targeting behaiour, so that
        // bots can potentially move away from other closer bots?
        HovercraftEntity *target = getTarget(botHovercraft, botPos);

        Action a;
        ai->getCurrentAction(target, bot, fTimeInSeconds, &a);

        executeAction(bot, a);

        bot->update(fTimeInSeconds);
    }
  
}
