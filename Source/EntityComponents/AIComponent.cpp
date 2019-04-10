#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */
#include "Physics/PhysicsManager.h"
#include "SpatialDataMap.h"
#include "EntityHeaders/HovercraftEntity.h"


/*
    Locations on the map that the AI can seek for.
*/
const vec2 seekPointsAI[] = {
    vec2(7,  7),    vec2(18,  7),   vec2(31,  7),
    vec2(7, 18),                    vec2(31, 18),
    vec2(7, 31),    vec2(18, 31),   vec2(31, 31)
};

#define SEEK_POINTS_SIZE 8

/*
    If the target enters the distance threshold of the bot, then the bot will
    be able to activate spikes.
*/
#define SPIKES_DISTANCE_THRESHOLD 15
/*
    If the target is within the distance threshold, there is a percent chance
    the bot will activate spikes.
*/
#define SPIKES_ACITVATION_CHANCE 50

#define ROCKET_ACCURACY_THRESHOLD 0.01
/*
    TODO document
*/
#define DISTANCE_BOX 15
#define CYCLE_TIME 7
#define MAX_TIME_TARGET CYCLE_TIME*4
/*
    If the target enters the chase distance, the bot will change to chase mode.
    Otherwise, the bot will go into seek mode.
*/
#define CHASE_DISTANCE 125

/*
    As the AI sets the position of the bot hovercrafts, we must ensure that the
    movement speed is inline with the players.
*/
#define MOVEMENT_RATE 20

AIComponent::AIComponent(int iEntityID, int iComponentID)
    : EntityComponent(iEntityID, iComponentID)
{
    for (int i = 0; i < 10; i++) {
        modeSequence[i] = static_cast<eMode>(FuncUtils::random(3) % 3);
        durations[i] = static_cast<int>(FuncUtils::random(20) + (modeSequence[i] == MODE_CHASE ? 10 : 5));
    }

    timeChased = static_cast<float>(FuncUtils::random(MAX_TIME_TARGET));

    m_pSpatialDataMap = SPATIAL_DATA_MAP;
}

// @AustinEaton : Function not Implemented
// void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPosition, glm::vec3 botVel, float botRotation) {
    //for (int i = 0; i < GA_ITERATIONS_PER_FRAME * 50; i++) {
   //     performMutation(targetPosition, playerVel, botPosition, botVel, botRotation, 0);
   // }
// }

AIComponent::~AIComponent() {
    // Nothing to destruct
}

// @Deprecated, use glm::distance(vec3 a, vec3 b)
float getDistance(vec3 a, vec3 b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}

// @Deprecated, use glm::distance(vec2 a, vec2 b)
float getDistance(vec2 a, vec2 b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}
/*
    Get the path for seek mode.

    @return the seek path

    @modifies seekLocation
    @modifies lastIndex

*/
vector<uvec2> AIComponent::getSeekPath()
{
    if (glm::distance(vec2(minXBot, minYBot), seekLocation) < 2) {
        vec3 currSeekLock = get2ndNearestSeekPoint(vec2(minXBot, minYBot));
        seekLocation = vec2(currSeekLock.x, currSeekLock.y);
        lastIndex = static_cast<int>(currSeekLock.z);
    }
    return m_pSpatialDataMap->getShortestPath(seekLocation,
                                              seekLocation,
                                              vec2(minXBot, minYBot),
                                              vec2(maxXBot, maxYBot));
}

/*
    Get the path for chase mode.

    @param target   to chase
*/
vector<uvec2> AIComponent::getChasePath() const
{
    return m_pSpatialDataMap->getShortestPath(vec2(minXTarget, minYTarget),
                                                vec2(maxXTarget, maxYTarget),
                                                vec2(minXBot + 1, minYBot + 1),
                                                vec2(maxXBot + 1, maxYBot + 1));
}
/*
    Update the target and bot locations on the spatial data map.

    @modifies minXTarget, maxXTarget, minYTarget, maxYTarget
    @modifies minXBot, maxXBot, minYBot, maxYBot
*/
void AIComponent::updateBotAndTargetLocations(const HovercraftEntity* target,
                                              const HovercraftEntity* bot)
{
    m_pSpatialDataMap->getMapIndices(target, &minXTarget, &maxXTarget,
                                             &minYTarget, &maxYTarget);
    m_pSpatialDataMap->getMapIndices(bot, &minXBot, &maxXBot,
                                          &minYBot, &maxYBot);
}

/*
    The the current mode determines the path to choose.

    @modifies path
*/
void AIComponent::determinePath()
{
    switch (m_eCurrentMode) {
    case MODE_CHASE:
        path = getChasePath();
        break;
    default:
        path = getSeekPath();
        break;
    }
}
/*
    Determinw which mode the AI should be in.

    @modifies seekLocation
    @modifies lastIndex
    @modifies m_eCurrentMode
*/
void AIComponent::determineMode(float distanceToTarget)
{
    if (timeChased > durations[currentActionNum]) {
        if (modeSequence[currentActionNum] != MODE_CHASE) {
            if (m_eCurrentMode == MODE_CHASE) {
                vec3 currSeekLock = getNearestSeekPoint(vec2(minXBot, minYBot));
                seekLocation = vec2(currSeekLock.x, currSeekLock.y);
                lastIndex = (int)currSeekLock.z;
            }
        }
        currentActionNum = (currentActionNum + 1) % 10;
        timeChased = 0;
        m_eCurrentMode = modeSequence[currentActionNum];
    }
}

/*
    Determine the global position in world space the bot should be.
    This will modifiy the global position of the bot.

    @modifies bot   position
*/
void AIComponent::determinePosition(HovercraftEntity *bot,
                                    const vec3 &botPos,
                                    float fTimeInSeconds)
{
    vec2 differenceSum = vec2(0,0);

    if ((botPos.x - seekPoint.x) > 0) {
        // Move left
        differenceSum.x += MOVEMENT_RATE * -1;
    } else {
        // Move right
        differenceSum.x += MOVEMENT_RATE;
    }

    if ((botPos.z - seekPoint.z) > 0) {
        // Move back
        differenceSum.y += MOVEMENT_RATE * -1;
    } else {
        // Move right
        differenceSum.y += MOVEMENT_RATE;
    }

    // move bot based off distance sum
    bot->setPosition(vec2(botPos.x + differenceSum.x * fTimeInSeconds,
                          botPos.z + differenceSum.y * fTimeInSeconds));
}

/*
    Determine the turn angle, as well as if the bot should fire a rocket.

    @modifies a
*/
void AIComponent::determineTurn(const HovercraftEntity *bot,
                                const vec3 &distanceVectorToTarget,
                                const vec3 &botDirectionVector,
                                Action *a)
{
    // Normalize
    vec3 normalizedBotDirection = glm::normalize(botDirectionVector);
    vec3 normalizedDistanceVectorToTarget = glm::normalize(distanceVectorToTarget);

    // @Austin previously this was calculated with the vec3::length() method.
    // The vec3::length() method does not return the length (magnitude) of the
    // vector, but rather the number of elements the vec3 contains, which in
    // this case for a vec3 is ALWAYS 3. (If you look at the definition of
    // glm::vec3::length(), it's actually hardcoded to return the value 3. I'm
    // not sure if this was your intention to divide bot direction vector and
    // the distance to target vectors by 3.

    // Old code:
    // botDirectionVector /= botDirectionVector.length(); // same as botDirectionVector /= 3;
    // distanceVectorToTarget /= distanceVectorToTarget.length();

    float mSlope = normalizedDistanceVectorToTarget.z / normalizedDistanceVectorToTarget.x;
    float yVal = mSlope * normalizedBotDirection.x;

    // So this will be either -1 or 1?
    int XvalMul = static_cast<int>((normalizedDistanceVectorToTarget.x / abs(normalizedDistanceVectorToTarget.x)));

    float accuracy = abs(yVal - botDirectionVector.z);
    a->shouldFireRocket = shouldFireRocket(bot, accuracy);

    // @Austin Is the order intentional?
    // ie. if the bot should fire a rocket, they won't if the first if statement case
    // is true, since this is an if-else-if chain.
    // I changed it so that the shouldFireRocket is set beforehand, independent of this
    // if-else-if chain.
    if (yVal > botDirectionVector.z) {
        // Turn right
        a->turn = static_cast<float>(1 * XvalMul);
    } else if (a->shouldFireRocket) {
        // Don't turn, and shoot
        a->turn = 0.0f;
    } else {
        // Turn left
        a->turn = static_cast<float>(-1 * XvalMul);
    }
}

/*
    To fire the rocket, the accuracy should be under the
    ROCKET_ACCURACY_THRESHOLD, and the bot must be in chase mode.
*/
bool AIComponent::shouldFireRocket(const HovercraftEntity *bot,
                                   float accuracy)
{
    return (accuracy < ROCKET_ACCURACY_THRESHOLD)
        && m_eCurrentMode == MODE_CHASE
        && bot->isOffCooldown(eAbility::ABILITY_ROCKET);
}

/*
    If the target is inside the activation threshold, there is a random chance
    spikes will activate.
*/
bool AIComponent::shouldActivateSpikes(const HovercraftEntity *bot,
                                       float distanceToTarget)
{
    return distanceToTarget <= SPIKES_DISTANCE_THRESHOLD
        && FuncUtils::random(1, 100) <= SPIKES_ACITVATION_CHANCE
        && bot->isOffCooldown(eAbility::ABILITY_SPIKES);
}

// @note bot may be used later
bool AIComponent::shouldActivateTrail(const HovercraftEntity *bot)
{
    // @Austin explain why should the trail activate if and only if in seek
    // mode?
    return m_eCurrentMode == MODE_SEEK;
}

/*
    If the target is too far away, (beyond the CHASE_DISTANCE), or the time
*/
bool AIComponent::shouldChooseSeekMode(float distanceToTarget)
{
    return distanceToTarget > CHASE_DISTANCE
        || timeChased < CYCLE_TIME;
}
/*
    TODO maybe make it go to center?
    Given the current position, get the 2nd closest seek point

    @param currentPos   of the AI
    @return the seek position that is closest to the currentPos
*/
vec3 AIComponent::get2ndNearestSeekPoint(vec2 currentPos) const {
    vec2 nearest = currentPos;
    vec2 nearest2nd = currentPos;
    float distance = numeric_limits<float>::max();
    float distance2 = numeric_limits<float>::max();
    int lowestIndex = -1;
    for (int i = 0; i < SEEK_POINTS_SIZE; i++) {
        float currDis = glm::distance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y),
                                      vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance && i != lastIndex) {
            if (distance < distance2) {
                nearest2nd = nearest;
                distance2 = distance;
                lowestIndex = i;
            }
            nearest = seekPointsAI[i];
            distance = currDis;
        } else if (currDis < distance2 && i != lastIndex) {
            nearest2nd = seekPointsAI[i];
            distance2 = currDis;
            lowestIndex = i;
        }
    }
    return vec3(seekPointsAI[static_cast<eMode>(FuncUtils::random(8) % 8)], lowestIndex);
}

/*
    Given the current position, get the closest seek point

    @param currentPos   of the AI
    @return the seek position that is closest to the currentPos
*/
vec3 AIComponent::getNearestSeekPoint(vec2 currentPos) const {
    vec2 nearest = currentPos;
    float distance = numeric_limits<float>::max();
    int lastLoc = -1;
    for (int i = 0; i < SEEK_POINTS_SIZE; i++) {
        float currDis = glm::distance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y),
                                      vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance) {
            nearest = seekPointsAI[i];
            distance = currDis;
            lastLoc = i;
        }
    }
    return vec3(nearest, lastLoc);
}

/*
    @NOTE
    I have no idea what's going on here. The seek point is being updated, but
    according to what? Nearest? What makes this different than
    getNearestSeekPoint

    @param botPosition           of bot to determine seek point

    @modifies seekPoint     based on what?
*/
void AIComponent::updateSeekPoint(const vec3 &botPos)
{
    vec2 offset = m_pSpatialDataMap->getWorldOffset();
    float tileSize = m_pSpatialDataMap->getTileSize();
    glm::vec3 nextPos = vec3(0, 0, 0);
    if (path.size() >= 2) {
        // get path position that is sufficently far away for seek point.
        for (int i = 0; i < (int)path.size(); i++) { 
            nextPos = vec3(path.at(i).x * tileSize + offset.x,
                           0,
                           path.at(i).y * tileSize + offset.y);
            if (abs((botPos - nextPos).x) + abs((botPos - nextPos).z) > DISTANCE_BOX) {
                break;
            }
        }
        seekPoint = nextPos;
    }
}
/*
    @param target           hovercraft the AI is to target. Could be a player or bot.
    @param bot              corresponding to this AIComponent
    @param fTimeInSeconds   time since last update in seconds

    @return a               the action the AI should act upon. Represnts all
                            the actions that should be performed by the
                            specified bot.

    @NOTE: Handles the movement of the bot entity so that part of the action
           should be dissregarded. however it does not handle the lanuching of
           rockets and abilities, that should be handled by the caller.


*/
void AIComponent::getCurrentAction(HovercraftEntity *target,
                                   HovercraftEntity *bot,
                                   float fTimeInSeconds,
                                   Action *a) {

    // not sure if an array in a struct is deep or shallow copied
    //memcpy(a, &frames[currentBest][currentPlace], sizeof(Action));

    // Seet zero ability usage so they aren't used
    const vec3 botPosition = bot->getPosition();
    const vec3 targetPosition = target->getPosition();
    // Scalar value
    const float distanceToTarget = glm::distance(targetPosition, botPosition);
    // Includes sign, based on world space coordinates
    vec3 distanceVectorToTarget = targetPosition - botPosition;

    // Direction vector that the bot is facing
    vec3 botDirectionVector;
    bot->getDirectionVector(&botDirectionVector);

    updateBotAndTargetLocations(target, bot);

    determinePath();

    updateSeekPoint(botPosition);

    determineMode(distanceToTarget);

    determineTurn(bot, distanceVectorToTarget, botDirectionVector, a);
    determinePosition(bot, botPosition, fTimeInSeconds);

    a->shouldActivateSpikes = shouldActivateSpikes(bot, distanceToTarget);
    a->shouldActivateTrail = true; //shouldActivateTrail(bot);
}

void AIComponent::update(float fTimeInSeconds)
{
    timeChased += fTimeInSeconds;
}
