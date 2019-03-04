#pragma once
#include "EntityComponentHeaders/EntityComponent.h"
#include <vector>
/*
For collisions
*/
#define MUTATIONS_PER_FRAME 3
#define GA_ITERATIONS_PER_FRAME 50
#define LOOK_AHEAD_FRAMES 30
#define MUTATION_SET 10
#define ROCKET_VEL 10
#define ROCKET_FRAME_PREDICTION_THRESHOLD 50
#define DISTANCE_REDUCTION_EVAL (1.0f/20000.0f)
/**************************************************************
 * Name: AIComponent
 * Written by: Austin Eaton
\***************************************************************************************/

// the below are the values for the 4 values
// fire Rocket, right-left turn, forward-back move,right-left move
struct Action {
    float actionsToTake[5] = { 0,0,0,0,0 };
};
class AIComponent :
    public EntityComponent
{
public:
    // Constructor/Destructor - Destructor must be virtual for proper deletion through unique_ptrs
    AIComponent(int iEntityID, int iComponentID);
    virtual ~AIComponent();
    void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation);
    // After Initialization, the update function should handle things for the Physics of this Entity.
    //    It's not necessarily necessary for the physics component to push information to the Entity as it updates, 
    //    moreso for it to hold and manage the physics information for the entity while providing functions that allow
    //    the entity to query their physics component for desired information.
    void update(float fTimeDeltaInMilliseconds); // Overloading Parent's virtual update function
    void popCurrentAction(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown, Action *a);
    // Various initialization functions as needed.
    // this function will allow Entities to retrieve the Transform Matrix required to modify their mesh.
private:
    void genRandomAction(Action *action);
    void performMutation(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown);
    void mutateSet(int setIndex, int mutations);
    float evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown);
    Action frames[MUTATION_SET][LOOK_AHEAD_FRAMES];
    int currentBest = 0;
    float currentBestEval = 0;
    int currentPlace = 0;
    int iEntityID;
    int iComponentID;
    float coolDown;
};

