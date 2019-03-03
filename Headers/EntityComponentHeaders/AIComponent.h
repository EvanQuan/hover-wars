#pragma once
#include "EntityComponentHeaders/EntityComponent.h"
#include <vector>
/*
For collisions
*/
#define MUTATIONS_PER_FRAME 10
#define LOOK_AHEAD_FRAMES 30
#define MUTATION_SET 10
#define ROCKET_VEL 10
#define ROCKET_FRAME_PREDICTION_THRESHOLD 50
/**************************************************************
 * Name: AIComponent
 * Written by: Austin Eaton
\***************************************************************************************/

// fire Rocket, right-left turn, forward-back move,right-left move
struct Action {
    int actionsToTake[4] = { 0,0,0,0 };
};
class AIComponent :
    public EntityComponent
{
public:
    // Constructor/Destructor - Destructor must be virtual for proper deletion through unique_ptrs
    AIComponent(int iEntityID, int iComponentID);
    virtual ~AIComponent();

    // After Initialization, the update function should handle things for the Physics of this Entity.
    //    It's not necessarily necessary for the physics component to push information to the Entity as it updates, 
    //    moreso for it to hold and manage the physics information for the entity while providing functions that allow
    //    the entity to query their physics component for desired information.
    void update(float fTimeDeltaInMilliseconds); // Overloading Parent's virtual update function

    // Various initialization functions as needed.
    // this function will allow Entities to retrieve the Transform Matrix required to modify their mesh.
private:
    Action genRandomAction();
    void mutateSet(int setIndex, int mutations);
    float evaluateSet(int setIndex, glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown);
    Action popCurrentAction(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation, float CurrcoolDown);
    Action frames[MUTATION_SET][LOOK_AHEAD_FRAMES];
    int currentBest = 0;
    float currentBestEval = 0;
    int currentPlace = 0;
    int iEntityID;
    int iComponentID;
};

