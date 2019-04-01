#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */
#include "Physics/PhysicsManager.h"
#include "SpatialDataMap.h"
#include "EntityHeaders/HovercraftEntity.h"


/*
    Locations on the map that the AI can seek for.
    @TODO describe what these are for/what they mean

    // Original order, if it matters?
    // vec2(18,7),     vec2(31,18),    vec2(7,18),     vec2(18,31),
    // vec2(31,31),    vec2(7,7),      vec2(31,7),     vec2(7,31)
*/
const vec2 seekPointsAI[] = {
    vec2(7,  7),    vec2(18,  7),   vec2(31,  7),
    vec2(7, 18),                    vec2(31, 18),
    vec2(7, 31),    vec2(18, 31),   vec2(31, 31)
};

#define SEEK_POINTS_SIZE 8
#define ACCURACY_THRESHOLD 0.01
#define DISTANCE_BOX 15
#define CYCLE_TIME 7
#define MAX_TIME_TARGET CYCLE_TIME*4
#define PROC_DISTANCE 125
#define MOVEMENT_RATE 20

AIComponent::AIComponent(int iEntityID, int iComponentID) : EntityComponent(iEntityID, iComponentID)
{

    ////for (int j = 0; j < MUTATION_SET; j++) {
    //    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
    //        genRandomAction(&frames[j][i]);
    //    }
    //}

    timeChased = static_cast<float>(FuncUtils::random(MAX_TIME_TARGET));
}

// @AustinEaton : Function not Implemented
void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation) {
    //for (int i = 0; i < GA_ITERATIONS_PER_FRAME * 50; i++) {
   //     performMutation(playerPos, playerVel, botPos, botVel, botRotation, 0);
   // }
}
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
    TODO maybe make it go to center?
    Given the current position, get the 2nd closest seek point

    @param currentPos   of the AI
    @return the seek position that is closest to the currentPos
*/
vec3 AIComponent::get2ndNearestSeekPoint(vec2 currentPos) {
    vec2 nearest = currentPos;
    vec2 nearest2nd = currentPos;
    float distance = numeric_limits<float>::max();
    float distance2 = numeric_limits<float>::max();
    int lowestIndex = -1;
    for (int i = 0; i < SEEK_POINTS_SIZE; i++) {
        float currDis = glm::distance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y), vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance && i != LastIndex) {
            if (distance < distance2) {
                nearest2nd = nearest;
                distance2 = distance;
                lowestIndex = i;
            }
            nearest = seekPointsAI[i];
            distance = currDis;
        } else if (currDis < distance2 && i != LastIndex) {
            nearest2nd = seekPointsAI[i];
            distance2 = currDis;
            lowestIndex = i;
        }
    }
    return vec3(nearest2nd, lowestIndex);
}

/*
    Given the current position, get the closest seek point

    @param currentPos   of the AI
    @return the seek position that is closest to the currentPos
*/
vec3 AIComponent::getNearestSeekPoint(vec2 currentPos) {
    vec2 nearest = currentPos;
    float distance = numeric_limits<float>::max();
    int lastLoc = -1;
    for (int i = 0; i < SEEK_POINTS_SIZE; i++) {
        float currDis = glm::distance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y), vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance) {
            nearest = seekPointsAI[i];
            distance = currDis;
            lastLoc = i;
        }
    }
    return vec3(nearest, lastLoc);
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

    //memcpy(a, &frames[currentBest][currentPlace], sizeof(Action));// not sure if an array in a struct is deep or shallow copied

    // Seet zero ability usage so they aren't used
    a->shouldFireRocket = false;
    a->shouldActivateTrail = false;
    glm::vec3 botPos = bot->getPosition();
    glm::vec3 botVel = bot->getLinearVelocity();

    unsigned int minXBot, minYBot, maxXBot, maxYBot;
    SPATIAL_DATA_MAP->getMapIndices(bot, &minXBot, &maxXBot, &minYBot, &maxYBot);// get bot loc on grid

    if (currentState == 0) { // if current state is chase, then get our path based on the player
        unsigned int minXPlayer, minYPlayer, maxXPlayer, maxYPlayer;
        SPATIAL_DATA_MAP->getMapIndices(target, &minXPlayer, &maxXPlayer, &minYPlayer, &maxYPlayer);
        path = SPATIAL_DATA_MAP->modifiedDikjistras(vec2(minXPlayer, minYPlayer), vec2(maxXPlayer, maxYPlayer), vec2(minXBot+1, minYBot+1), vec2(maxXBot+1, maxYBot+1));
    } else if (currentState == 1) {// if current state is seek the look for a nearest point.
        if (glm::distance(vec2(minXBot, minYBot), seekLocation) < 2) {
            vec3 currSeekLock = get2ndNearestSeekPoint(vec2(minXBot, minYBot));
            seekLocation = vec2(currSeekLock.x, currSeekLock.y);
            LastIndex = (int)currSeekLock.z;
        }

        path = SPATIAL_DATA_MAP->modifiedDikjistras(seekLocation, seekLocation, vec2(minXBot, minYBot), vec2(maxXBot, maxYBot));
    }
    vec2 offset = SPATIAL_DATA_MAP->getWorldOffset();

    glm::vec3 nextPos = vec3(0, 0, 0);
    if (path.size() >= 2) {
        for (int i = 0; i < (int)path.size(); i++) { // get path position that is sufficently far away for seek point.
            nextPos = vec3(path.at(i).x * SPATIAL_DATA_MAP->getTileSize() + offset.x, 0, path.at(i).y * SPATIAL_DATA_MAP->getTileSize() + offset.y);
            if (abs((botPos - nextPos).x) + abs((botPos - nextPos).z) > DISTANCE_BOX) {
                break;
            }
        }
        seekPoint = nextPos;
    }
    vec3 difference = target->getPosition() - botPos;
    float distanceToTarget = glm::distance(target->getPosition(), botPos);

    if (distanceToTarget > PROC_DISTANCE || timeChased < CYCLE_TIME) {
        if (currentState != 1) {
            vec3 currSeekLock = getNearestSeekPoint(vec2(minXBot, minYBot));
            seekLocation = vec2(currSeekLock.x, currSeekLock.y);
            LastIndex = (int)currSeekLock.z;
        }
        currentState = 1;
        nextPosMove = true;
    }
    else {
        currentState = 0;
        nextPosMove = false;
    }
    if (timeChased > MAX_TIME_TARGET) {
        timeChased = 0;
    }
    vec3 dirVector;
    bot->getDirectionVector(&dirVector);
    dirVector /= dirVector.length();
    difference /= difference.length();

    float mSlope = difference.z / difference.x;
    float yVal = mSlope * dirVector.x;


    glm::vec3 playerPos = target->getPosition();
    int XvalMul = (int)(difference.x / abs(difference.x));

    if (yVal > dirVector.z) {
        a->turn = static_cast<float>(1 * XvalMul);
    }
    else if (abs(yVal - dirVector.z) < ACCURACY_THRESHOLD && currentState == 0) {
        a->turn = 0.0f;
        a->shouldFireRocket = true;
    }
    else {
        a->turn = static_cast<float>(-1 * XvalMul);
    }
    bool isXdistance = false;
    vec2 differenceSum = vec2(0,0);
    if ((botPos.x - seekPoint.x) > 0) {
        differenceSum.x += MOVEMENT_RATE * -1;
    }
    else {
        differenceSum.x += MOVEMENT_RATE;
    }
    if ((botPos.z - seekPoint.z) > 0) {
        differenceSum.y += MOVEMENT_RATE * -1;
    }
    else {
        differenceSum.y += MOVEMENT_RATE;
    }
    if (nextPosMove) {
        a->shouldActivateTrail = true;
    }
    bot->setPosition(vec2(botPos.x + differenceSum.x * fTimeInSeconds, botPos.z + differenceSum.y * fTimeInSeconds)); // move player based off distance sum
    botPos = bot->getPosition();
}
void AIComponent::update(float fTimeInSeconds)
{
    timeChased += fTimeInSeconds;
}
