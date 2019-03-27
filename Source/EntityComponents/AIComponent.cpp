#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */
#include "Physics/PhysicsManager.h"
#include "SpatialDataMap.h"
#include "EntityHeaders/HovercraftEntity.h"


const vec2 seekPointsAI[] = { vec2(18,7),vec2(31,18),vec2(7,18),vec2(18,31),
vec2(31,31),vec2(7,7),vec2(31,7),vec2(7,31) };
#define ACCURACY_THRESHOLD 0.01
#define DISTANCE_BOX 5
#define CYCLE_TIME 7
#define MAX_TIME_TARGET CYCLE_TIME*4
#define PROC_DISTANCE 125
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

void AIComponent::genRandomAction(Action *action) {
    action->actionsToTake[0] = (float)(rand() % 2);
    action->actionsToTake[1] = (float)(rand() % 3 - 1);
    action->actionsToTake[2] = (float)(rand() % 3 - 1);
    action->actionsToTake[3] = (float)(rand() % 3 - 1);
    action->actionsToTake[4] = (float)(rand() % 2);
}
void AIComponent::mutateSet(int setIndex, int mutations) {
    for (int i = 0; i < mutations; i++) {
        int randomIndex = rand() % LOOK_AHEAD_FRAMES;
        genRandomAction(&frames[setIndex][randomIndex]);
    }

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
    for (int i = 0; i < seekPointsAI->length(); i++) {
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
    for (int i = 0; i < seekPointsAI->length(); i++) {
        float currDis = getDistance(vec3(seekPointsAI[i].x, 0, seekPointsAI[i].y), vec3(currentPos.x, 0, currentPos.y));
        if (currDis < distance) {
            nearest = seekPointsAI[i];
            distance = currDis;
            lastLoc = i;
        }
    }
    return vec3(nearest, lastLoc);
}

// @AustinEaton : CurrcoolDown not referenced.
// @AustinEaton : botVel not referenced.
float AIComponent::evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown) {
    float evaluation = 0;
    float framesTaken = 1;
    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
        // botPos.x += frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[2] * 10;
        // botPos.y += frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[3] * 10;
        float rot = frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[1] * 0.01f;
        rot = rot - (float)(floor(rot * (2 * 3.14159)) / (2 * 3.14159));
        botRotation += rot;
        //if (frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[0] == 1) {
        glm::vec3 rocketDir = glm::vec3(cos(botRotation), sin(botRotation), 0);
        // if (playerVel.x == 0) {
        playerVel.x = 0.00001f;
        // }
         //if (playerVel.y == 0) {
        playerVel.y = 0.00001f;
        //}
        float mRocket = rocketDir.y / rocketDir.x;
        float mPlayer = playerVel.y / playerVel.x;
        float negXRocket = -1 * mRocket * botPos.x;
        float negXPlayer = -1 * mPlayer * playerPos.x;
        float rocketTotal = negXRocket + botPos.y;
        float playerTotal = negXPlayer + playerPos.y;
        float totalAmount = rocketTotal - playerTotal;
        float totalM = mPlayer - mRocket;
        float intersectionX = totalAmount / totalM;//good old algebra for line intersections
        vec3 intersectionPoint = vec3(intersectionX, intersectionX * mPlayer + negXPlayer + playerPos.y, 0);
        vec3 difference = botPos - playerPos;
        difference /= difference.length();
        framesTaken = ((intersectionPoint - playerPos).x / playerVel.x) - ((intersectionPoint - botPos).x / playerVel.x);
        if ((intersectionX < 0 && rocketDir.x < 0) || (intersectionX > 0 && rocketDir.x > 0)) {
            //evaluation = 50 / framesTaken;
        }

        //}
    }
    evaluation = 50 / framesTaken;

    //evaluation += (playerPos - botPos).length() * DISTANCE_REDUCTION_EVAL;
    return evaluation;
}

// @AustinEaton : botRotation not referenced.
// @AustinEaton : botVel not referenced.
// @AustinEaton : playerVel not referenced.
void AIComponent::popCurrentAction(HovercraftEntity *mPlayer, HovercraftEntity *bot, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, Action *a) {

    //memcpy(a, &frames[currentBest][currentPlace], sizeof(Action));// not sure if an array in a struct is deep or shallow copied
    a->actionsToTake[ACTION_FIRE_ROCKET] = 0;
    a->actionsToTake[ACTION_FLAMETRAIL] = 0;

    unsigned int minXBot, minYBot, maxXBot, maxYBot;
    SPATIAL_DATA_MAP->getMapIndices(bot, &minXBot, &maxXBot, &minYBot, &maxYBot);
    if (currentState == 0) {
        unsigned int minXPlayer, minYPlayer, maxXPlayer, maxYPlayer;
        SPATIAL_DATA_MAP->getMapIndices(mPlayer, &minXPlayer, &maxXPlayer, &minYPlayer, &maxYPlayer);
        path = SPATIAL_DATA_MAP->modifiedDikjistras(vec2(minXPlayer, minYPlayer), vec2(maxXPlayer, maxYPlayer), vec2(minXBot, minYBot), vec2(maxXBot, maxYBot));
    }
    else if (currentState == 1) {
        if (getDistance(vec2(minXBot, minYBot), seekLocation) < 2) {
            vec3 currSeekLock = get2ndNearestSeekPoint(vec2(minXBot, minYBot));
            seekLocation = vec2(currSeekLock.x, currSeekLock.y);
            LastIndex = (int)currSeekLock.z;
        }

        path = SPATIAL_DATA_MAP->modifiedDikjistras(seekLocation, seekLocation, vec2(minXBot, minYBot), vec2(maxXBot, maxYBot));
    }
    vec2 offset = SPATIAL_DATA_MAP->getWorldOffset();
    glm::vec3 nextPos = vec3(0, 0, 0);//mPlayer->getPosition();
    if (path.size() >= 2) {
        for (int i = 0; i < (int)path.size(); i++) {
            nextPos = vec3(path.at(i).x * SPATIAL_DATA_MAP->getTileSize() + offset.x, 0, path.at(i).y * SPATIAL_DATA_MAP->getTileSize() + offset.y);
            if (abs((botPos - nextPos).x) + abs((botPos - nextPos).z) > DISTANCE_BOX) {
                break;
            }
        }
        seekPoint = nextPos;
    }
    vec3 difference = mPlayer->getPosition() - botPos;
    float distanceToTarget = getDistance(mPlayer->getPosition(), botPos);
    //std::cout << "Distance to Target: " << distanceToTarget << std::endl;
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

    bool isXdistance = false;   // @AustinEaton : Initialized but not referenced.
    if ((botPos.x - seekPoint.x) > 0) {
        a->actionsToTake[2] = -1;
    }
    else {
        a->actionsToTake[2] = 1;
    }
    if ((botPos.z - seekPoint.z) > 0) {
        a->actionsToTake[3] = -1;
    }
    else {
        a->actionsToTake[3] = 1;
    }
    if (nextPosMove) {
        a->actionsToTake[eAction::ACTION_FLAMETRAIL] = 1;
    }
    //a->actionsToTake[2] = difference.x/100.0f;
    //a->actionsToTake[3] = difference.z/100.0f;
    // std::cout << "bot rotation: "<< (botPos.z - seekPoint.z) <<"                                               " << angle << std::endl;
#ifndef NDEBUG
    //std::cout << "difference y: " << difference.x << " y: " << difference.y << " z: " << difference.z << std::endl;
    //currentPlace = (1 + currentPlace) % LOOK_AHEAD_FRAMES;
#endif
}
void AIComponent::performMutation(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown) {
    for (int i = 0; i < MUTATION_SET; i++) {
        if (i != currentBest) {
            //sets all others sets equal to the current best set
            memcpy(frames[i], frames[currentBest], sizeof(Action) * LOOK_AHEAD_FRAMES);
            //mutates each set
            mutateSet(i, MUTATIONS_PER_FRAME);

            float eval = evaluateSet(i, playerPos, playerVel, botPos, botVel, botRotation, CurrcoolDown);

            if (eval > currentBestEval) {
                currentBest = i;
                currentBestEval = eval;
            }
        }
    }
}
void AIComponent::update(float fTimeInSeconds)
{
    timeChased += fTimeInSeconds;
}
