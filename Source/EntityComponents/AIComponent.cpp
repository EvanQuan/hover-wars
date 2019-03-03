#include "EntityComponentHeaders/AIComponent.h"
#include <time.h>       /* time */


AIComponent::AIComponent(int iEntityID, int iComponentID) : EntityComponent(iEntityID, iComponentID)
{
    for (int j = 0; j < MUTATION_SET; j++) {
        for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
            frames[j][i] = genRandomAction();
        }
    }
    /* initialize random seed: */
    srand(time(NULL));

}
AIComponent::~AIComponent() {

}
Action AIComponent::genRandomAction() {
    return  { {rand() % 2, rand() % 3 -1,rand() % 3 - 1,rand() % 3 - 1} };
}
void AIComponent::mutateSet(int setIndex,int mutations) {
    for (int i = 0; i < mutations; i++) {
        int randomIndex = rand() % LOOK_AHEAD_FRAMES;
        frames[setIndex][randomIndex] = genRandomAction();
    }

}
float AIComponent::evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation,float CurrcoolDown) {
    int evaluation = 0;
    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
        botPos.x += frames[setIndex][i + currentBest % LOOK_AHEAD_FRAMES].actionsToTake[2] * 10;
        botPos.y += frames[setIndex][i + currentBest % LOOK_AHEAD_FRAMES].actionsToTake[3] * 10;
        botRotation = frames[setIndex][i + currentBest % LOOK_AHEAD_FRAMES].actionsToTake[1];
        if (CurrcoolDown < 0 && frames[setIndex][i + currentBest % LOOK_AHEAD_FRAMES].actionsToTake[0] == 1) {
            glm::vec3 rocketDir = glm::vec3(cos(botRotation),sin(botRotation),0);
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
            float framesTaken = (intersectionPoint - playerPos).x / playerVel.x;
            if (framesTaken < ROCKET_FRAME_PREDICTION_THRESHOLD) {
                if ((intersectionX < 0 && rocketDir.x < 0) || (intersectionX > 0 && rocketDir.x > 0)) {
                    evaluation += 50;
                }
            }
        }
    }
    return 0;
}
Action AIComponent::popCurrentAction(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown) {
    for (int i = 0; i < MUTATION_SET; i++) {
        if (i != currentBest) {
            //sets all others sets equal to the current best set
            memcpy(frames[i], frames[currentBest], sizeof(Action) * LOOK_AHEAD_FRAMES);
            //mutates each set
            mutateSet(i, MUTATIONS_PER_FRAME);

            float eval = evaluateSet(i,playerPos,playerVel, botPos, botVel,botRotation, CurrcoolDown);
        }
    }
    return frames[0][0];
}
void AIComponent::update(float fTimeDeltaInMilliseconds)
{
    
}
