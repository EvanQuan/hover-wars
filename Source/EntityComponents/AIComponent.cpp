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
    return  { {rand() % 2, rand() % 3} };
}
void AIComponent::mutateSet(int setIndex,int mutations) {
    for (int i = 0; i < mutations; i++) {
        int randomIndex = rand() % LOOK_AHEAD_FRAMES;
        frames[setIndex][randomIndex] = genRandomAction();
    }

}
float AIComponent::evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, glm::vec3 botRotation) {
    return 0;
}
Action AIComponent::popCurrentAction(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, glm::vec3 botRotation) {
    for (int i = 0; i < MUTATION_SET; i++) {
        if (i != currentBest) {
            //sets all others sets equal to the current best set
            memcpy(frames[i], frames[currentBest], sizeof(Action) * LOOK_AHEAD_FRAMES);
            //mutates each set
            mutateSet(i, MUTATIONS_PER_FRAME);

            float eval = evaluateSet(i,playerPos,playerVel, botPos, botVel,botRotation);
        }
    }
    return frames[0][0];
}
void AIComponent::update(float fTimeDeltaInMilliseconds)
{
    
}
