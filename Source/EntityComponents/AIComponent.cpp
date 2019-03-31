#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */
#include "Physics/PhysicsManager.h"
#include "SpatialDataMap.h"
#include "EntityHeaders/HovercraftEntity.h"


const vec2 seekPointsAI[] = { vec2(18,7),vec2(31,18),vec2(7,18),vec2(18,31),
vec2(31,31),vec2(7,7),vec2(31,7),vec2(7,31) };
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

    /* initialize random seed: */
    // @AustinEaton @EvanQuan : srand re-seeded for every AIComponent?
     srand(static_cast<unsigned int>(time(NULL)));
    // timeChased = rand() % (MAX_TIME_TARGET);
    timeChased = static_cast<float>(FuncUtils::random(0, MAX_TIME_TARGET - 1));
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
float getDistance(vec3 a, vec3 b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}
float getDistance(vec2 a, vec2 b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}
//TODO maybe make it go to center?
vec3 AIComponent::get2ndNearestSeekPoint(vec2 currentPos) {
    vec2 nearest = currentPos;
    vec2 nearest2nd = currentPos;
    float distance = 1000000;
    float distance2 = 1000000;
    int lowestIndex = -1;
    for (int i = 0; i < SEEK_POINTS_SIZE; i++) {
        float currDis = getDistance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y), vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance && i != LastIndex) {
            if (distance < distance2) {
                nearest2nd = nearest;
                distance2 = distance;
                lowestIndex = i;
            }
            nearest = seekPointsAI[i];
            distance = currDis;
        }
        else if (currDis < distance2 && i != LastIndex) {
            nearest2nd = seekPointsAI[i];
            distance2 = currDis;
            lowestIndex = i;
        }
    }
    return vec3(nearest2nd, lowestIndex);
}
vec3 AIComponent::getNearestSeekPoint(vec2 currentPos) {
    vec2 nearest = currentPos;
    float distance = 1000000;
    int lastLoc = -1;
    for (int i = 0; i < SEEK_POINTS_SIZE; i++) {
        float currDis = getDistance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y), vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance) {
            nearest = seekPointsAI[i];
            distance = currDis;
            lastLoc = i;
        }
    }
    return vec3(nearest, lastLoc);
}
/*
Function: getCurrentAction
returns: an Action a, this action represents all the actions that should be preformed by the bot in question

parameters:
mPlayer: this is a generic hovercraft entity, it is not nessicarally a player, it could be a bot. however
it is the hovercraft entity we are currently targeting. It should be the hovercraft to the bot entity.
bot: bot entity that relates to AI component class
delta_time: time difference since last update.

Explaination:
this function handles the movement of the bot entity so that part of the action should be dissregarded.
however it does not handle the lanuching of rockets and abilities, that should be handled by the caller.
*/
void AIComponent::getCurrentAction(HovercraftEntity *mPlayer, HovercraftEntity *bot,float delta_time, Action *a) {

    //memcpy(a, &frames[currentBest][currentPlace], sizeof(Action));// not sure if an array in a struct is deep or shallow copied
    a->actionsToTake[ACTION_FIRE_ROCKET] = 0;
    a->actionsToTake[ACTION_FLAMETRAIL] = 0;//zero abilities so they aren't used
    glm::vec3 botPos = bot->getPosition();
    glm::vec3 botVel = bot->getLinearVelocity();

    unsigned int minXBot, minYBot, maxXBot, maxYBot;
    SPATIAL_DATA_MAP->getMapIndices(bot, &minXBot, &maxXBot, &minYBot, &maxYBot);// get bot loc on grid

    if (currentState == 0) { // if current state is chase, then get our path based on the player
        unsigned int minXPlayer, minYPlayer, maxXPlayer, maxYPlayer;
        SPATIAL_DATA_MAP->getMapIndices(mPlayer, &minXPlayer, &maxXPlayer, &minYPlayer, &maxYPlayer);
        path = SPATIAL_DATA_MAP->modifiedDikjistras(vec2(minXPlayer, minYPlayer), vec2(maxXPlayer, maxYPlayer), vec2(minXBot+1, minYBot+1), vec2(maxXBot+1, maxYBot+1));
    }
    else if (currentState == 1) {// if current state is seek the look for a nearest point.
        if (getDistance(vec2(minXBot, minYBot), seekLocation) < 2) {
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
    vec3 difference = mPlayer->getPosition() - botPos;
    float distanceToTarget = getDistance(mPlayer->getPosition(), botPos);

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


    glm::vec3 playerPos = mPlayer->getPosition();
    int XvalMul = (int)(difference.x / abs(difference.x));

    if (yVal > dirVector.z) {
        a->actionsToTake[1] = (float)(1 * XvalMul);
    }
    else if (abs(yVal - dirVector.z) < ACCURACY_THRESHOLD && currentState == 0) {
        a->actionsToTake[1] = 0;
        a->actionsToTake[ACTION_FIRE_ROCKET] = 1;
    }
    else {
        a->actionsToTake[1] = (float)(-1 * XvalMul);
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
        a->actionsToTake[eAction::ACTION_FLAMETRAIL] = 1;
    }
    bot->setPosition(vec2(botPos.x + differenceSum.x * delta_time, botPos.z + differenceSum.y * delta_time)); // move player based off distance sum
    botPos = bot->getPosition();
}
void AIComponent::update(float fTimeInSeconds)
{
    timeChased += fTimeInSeconds;
}
