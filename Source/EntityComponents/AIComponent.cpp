#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */


AIComponent::AIComponent(int iEntityID, int iComponentID) : EntityComponent(iEntityID, iComponentID)
{
    for (int j = 0; j < MUTATION_SET; j++) {
        for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
            genRandomAction(&frames[j][i]);
        }
    }

    /* initialize random seed: */
    srand(static_cast<unsigned int>(time(NULL)));

}
void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation) {
    for (int i = 0; i < GA_ITERATIONS_PER_FRAME * 50; i++) {
        performMutation(playerPos, playerVel, botPos, botVel, botRotation, 0);
    }
}
AIComponent::~AIComponent() {

}
void AIComponent::genRandomAction(Action *action) {
    action->actionsToTake[0] = rand() % 2;
    action->actionsToTake[1] = rand() % 3 - 1;
    action->actionsToTake[2] = rand() % 3 - 1;
    action->actionsToTake[3] = rand() % 3 - 1;
}
void AIComponent::mutateSet(int setIndex,int mutations) {
    for (int i = 0; i < mutations; i++) {
        int randomIndex = rand() % LOOK_AHEAD_FRAMES;
        genRandomAction(&frames[setIndex][randomIndex]);
    }

}
float AIComponent::evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation,float CurrcoolDown) {
    float evaluation = 0;
    float framesTaken = 1;
    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
       // botPos.x += frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[2] * 10;
       // botPos.y += frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[3] * 10;
        float rot = frames[setIndex][i + currentPlace % LOOK_AHEAD_FRAMES].actionsToTake[1] * 0.01;
        rot = rot - floor(rot * (2 * 3.14159))/ (2 * 3.14159);
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
            framesTaken = (intersectionPoint - playerPos).x / playerVel.x;
            if ((intersectionX < 0 && rocketDir.x < 0) || (intersectionX > 0 && rocketDir.x > 0)) {
                //evaluation = 50 / framesTaken;
            }
            
        //}
    }
    evaluation = 50 / framesTaken;
    
    //evaluation += (playerPos - botPos).length() * DISTANCE_REDUCTION_EVAL;
    return evaluation;
}
#define DISTANCE_BOX 4
float timeChased = 0;
bool isChasing = false;
glm::vec3 seekPoint = vec3(200,0,30);
void AIComponent::popCurrentAction(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown, Action *a) {
    memcpy(a, &frames[currentBest][currentPlace], sizeof(Action));// not sure if an array in a struct is deep or shallow copied

    //for (int i = 0; i < MUTATION_SET; i++)
    //    genRandomAction(&frames[i][currentPlace]);//adds a random action to the end
    //for (int i = 0; i < GA_ITERATIONS_PER_FRAME; i++) {
    //    performMutation(playerPos, playerVel, botPos, botVel, botRotation, CurrcoolDown);
    //}
    vec3 difference = botPos - playerPos;
    difference.x = seekPoint.x;
    difference.z = seekPoint.z;
    difference.y = seekPoint.y;

    difference /= difference.length();
    float angle = atan2(difference.x, difference.z);
    double botAmount = botRotation + (3.1415926535 / 2);//(((botRotation + (3.1415926535 / 2)) / (3.1415926535 * 2)) - (floor((botRotation + (3.1415926535 / 2))/(3.1415926535 * 2)))* (3.1415926535 * 2));
    double modAmount = (botRotation / (3.1415926535 * 2) - floor(botRotation / (3.1415926535 * 2))) * (3.1415926535 * 2) - 3.1415926535;
   

    if (isChasing) {
        seekPoint = playerPos;
        if (timeChased > 5) {
            isChasing = false;
            seekPoint.x = (float)(rand() % 200 - 100);
            seekPoint.z = (float)(rand() % 200);
            timeChased = 0;
        }
    }else if (timeChased > 2) {
        std::cout << "chasing = true" << std::endl;
        isChasing = true;
        timeChased = 0;
    }

    if (angle - botRotation > 0.01) {
        a->actionsToTake[1] = -1;
    }
    else if (angle - botRotation < -0.01) {
        a->actionsToTake[1] = 1;
    }
    else {
        a->actionsToTake[1] = 0;
    }
    if ((botPos.x - seekPoint.x) > DISTANCE_BOX) {
        a->actionsToTake[2] = -1;
    }
    else if ((botPos.x - seekPoint.x) < -DISTANCE_BOX) {
        a->actionsToTake[2] = 1;
    }
    else {
        a->actionsToTake[2] = 0;
    }
    if ((botPos.z - seekPoint.z) > DISTANCE_BOX) {
        a->actionsToTake[3] = -1;
    }
    else if ((botPos.z - seekPoint.z) < -DISTANCE_BOX) {
        a->actionsToTake[3] = 1;
    }
    else {
        a->actionsToTake[3] = 0;
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
void AIComponent::update(float fTimeDeltaInMilliseconds)
{
    timeChased += fTimeDeltaInMilliseconds;
}
