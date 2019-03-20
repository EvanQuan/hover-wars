#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */
#include "Physics/PhysicsManager.h"
#include "SpatialDataMap.h"
#include "EntityHeaders/HovercraftEntity.h"

#define ACCURACY_THRESHOLD 0.01
AIComponent::AIComponent(int iEntityID, int iComponentID) : EntityComponent(iEntityID, iComponentID)
{
    for (int j = 0; j < MUTATION_SET; j++) {
        for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
            genRandomAction(&frames[j][i]);
        }
    }

    /* initialize random seed: */
    // srand(static_cast<unsigned int>(time(NULL)));

}
void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation) {
    for (int i = 0; i < GA_ITERATIONS_PER_FRAME * 50; i++) {
        performMutation(playerPos, playerVel, botPos, botVel, botRotation, 0);
    }
}
AIComponent::~AIComponent() {

}
void AIComponent::genRandomAction(Action *action) {
    //action->actionsToTake[ACTION_FIRE_ROCKET] = (float) (rand() % 2);
    //action->actionsToTake[ACTION_TURN] = (float) (rand() % 3 - 1);
    //action->actionsToTake[ACTION_MOVE_FORWARDS_BACKWARDS] = (float) (rand() % 3 - 1);
    //action->actionsToTake[ACTION_MOVE_RIGHT_LEFT] = (float) (rand() % 3 - 1);
    //action->actionsToTake[ACTION_SIZE] = (float) (rand() % 2);
    action->actionsToTake[ACTION_FIRE_ROCKET] = static_cast<float>(FuncUtils::random(0, 1));
    action->actionsToTake[ACTION_TURN] = static_cast<float>(FuncUtils::random(-1, 1));
    action->actionsToTake[ACTION_MOVE_FORWARDS_BACKWARDS] = static_cast<float>(FuncUtils::random(-1, 1));
    action->actionsToTake[ACTION_MOVE_RIGHT_LEFT] = static_cast<float>(FuncUtils::random(-1, 1));
    action->actionsToTake[ACTION_SIZE] = static_cast<float>(FuncUtils::random(0, 1));
}
void AIComponent::mutateSet(int setIndex,int mutations) {
    for (int i = 0; i < mutations; i++) {
        // int randomIndex = rand() % LOOK_AHEAD_FRAMES;
        int randomIndex = FuncUtils::random(0, LOOK_AHEAD_FRAMES - 1);
        genRandomAction(&frames[setIndex][randomIndex]);
    }

}
float AIComponent::evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation,float CurrcoolDown) {
    float evaluation = 0;
    float framesTaken = 1;
    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
       // botPos.x += frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[2] * 10;
       // botPos.y += frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[3] * 10;
        float rot = frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[1] * 0.01f;
        rot = rot - (float)(floor(rot * (PI_2))/ (PI_2));
        botRotation += rot;
        //if (frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[0] == 1) {
            glm::vec3 rocketDir = glm::vec3(cos(botRotation),sin(botRotation),0);
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
            vec3 intersectionPoint = vec3(intersectionX, intersectionX * mPlayer + negXPlayer + playerPos.y,0);
            vec3 difference = botPos-playerPos;
            difference/=difference.length();
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
#define DISTANCE_BOX 5
glm::vec3 seekPoint = vec3(200,0,30);
void AIComponent::popCurrentAction(HovercraftEntity *mPlayer,HovercraftEntity *bot, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, Action *a) {

    //memcpy(a, &frames[currentBest][currentPlace], sizeof(Action));// not sure if an array in a struct is deep or shallow copied

    //for (int i = 0; i < MUTATION_SET; i++)
    //    genRandomAction(&frames[i][currentPlace]);//adds a random action to the end
    //for (int i = 0; i < GA_ITERATIONS_PER_FRAME; i++) {
    //    performMutation(playerPos, playerVel, botPos, botVel, botRotation, CurrcoolDown);
    //}
    unsigned int minXPlayer, minYPlayer, maxXPlayer, maxYPlayer;
    SPATIAL_DATA_MAP->getMapIndices(mPlayer, &minXPlayer, &maxXPlayer, &minYPlayer, &maxYPlayer);
    unsigned int minXBot, minYBot, maxXBot, maxYBot;
    SPATIAL_DATA_MAP->getMapIndices(bot, &minXBot, &maxXBot, &minYBot, &maxYBot);
    //if (timeSinceLastUpdate > 1) {
    a->actionsToTake[ACTION_FIRE_ROCKET] = 0;
    path = SPATIAL_DATA_MAP->modifiedDikjistras(vec2(minXPlayer, minYPlayer), vec2(minXBot, minYBot));
    timeSinceLastUpdate = 0;
    vec2 offset = SPATIAL_DATA_MAP->getWorldOffset();
    //std::cout << "popCurrentAction: " << seekPoint.x << "," << seekPoint.z << std::endl;
    //}
    glm::vec3 nextPos = vec3(0, 0, 0);//mPlayer->getPosition();
            
    if (path.size() >= 2) {
        for (int i = 0; i < (int)path.size(); i++) {
            nextPos = vec3(path.at(1).x * SPATIAL_DATA_MAP->getTileSize() + offset.x, 0, path.at(1).y * SPATIAL_DATA_MAP->getTileSize() + offset.y);
            vec3 difference = botPos - nextPos;
            if (difference.length() > DISTANCE_BOX) {
                break;
            }
        }
    }

    vec3 difference = mPlayer->getPosition() - botPos;
    vec3 botToPlayer = nextPos - botPos;
    vec3 dirVector;
    bot->m_pPhysicsComponent->getDirectionVector(&dirVector);
    dirVector /= dirVector.length();
    botToPlayer /= botToPlayer.length();
    difference /= difference.length();

    float mSlope = difference.z / difference.x;
    float yVal = mSlope * dirVector.x;


    float angle = atan(difference.x,difference.z);
    double angleBetween = angle - botRotation < angle - botRotation + (PI_2) ? angle - botRotation : angle - botRotation + (3.1415926535 * 2);
    double botAmount = botRotation + PI_HALF;//(((botRotation + (3.1415926535 / 2)) / (3.1415926535 * 2)) - (floor((botRotation + (3.1415926535 / 2))/(3.1415926535 * 2)))* (3.1415926535 * 2));
    double modAmount = (botRotation / (PI_2) - floor(botRotation / (PI_2))) * (PI_2) - PI;

    seekPoint = nextPos;

    glm::vec3 playerPos = mPlayer->getPosition();
    int XvalMul = (int)(difference.x / abs(difference.x));

    if (yVal > dirVector.z) {
        a->actionsToTake[ACTION_TURN] = (float) (1 * XvalMul);
    }
    else if(abs(yVal - dirVector.z) < ACCURACY_THRESHOLD){
        a->actionsToTake[ACTION_TURN] = 0;
        a->actionsToTake[ACTION_FIRE_ROCKET] = 1;
    }
    else {
        a->actionsToTake[ACTION_TURN] = (float)(-1 * XvalMul);
    }
    
    bool isXdistance = false;
    if ((botPos.x - nextPos.x) > DISTANCE_BOX) {
        a->actionsToTake[ACTION_MOVE_FORWARDS_BACKWARDS] = -1;
    }
    else if ((botPos.x - nextPos.x) < -DISTANCE_BOX) {
        a->actionsToTake[ACTION_MOVE_FORWARDS_BACKWARDS] = 1;
    }
    else {
        isXdistance = true;
    }
    if ((botPos.z - nextPos.z) > DISTANCE_BOX) {
        a->actionsToTake[ACTION_MOVE_RIGHT_LEFT] = -1;
    }
    else if ((botPos.z - nextPos.z) < -DISTANCE_BOX) {
        a->actionsToTake[ACTION_MOVE_RIGHT_LEFT] = 1;
    }
    else if(isXdistance){
        a->actionsToTake[ACTION_SIZE] = 1;
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
