#pragma once
#include "EntityComponentHeaders/EntityComponent.h"
#include "EntityHeaders/HovercraftEntity.h"
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
// fire Rocket, right-left turn, forward-back move, right-left move
struct Action {
    // @NOTE Why size of 5 instead of 4?
    float actionsToTake[5] = { 0,0,0,0,0 };
    // float actionsToTake[4] = { 0,0,0,0 };
};
class AIComponent final :
    public EntityComponent
{
public:
    // For Action indexing
    enum eAction {
        ACTION_FIRE_ROCKET = 0,
        ACTION_TURN,
        ACTION_MOVE_FORWARDS_BACKWARDS,
        ACTION_MOVE_RIGHT_LEFT,
        ACTION_FLAMETRAIL,
    };
    // Constructor/Destructor - Destructor must be virtual for proper deletion through unique_ptrs
    AIComponent(int iEntityID, int iComponentID);
    ~AIComponent();
    void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPos, glm::vec3 botVel, float botRotation);
    // After Initialization, the update function should handle things for the Physics of this Entity.
    //    It's not necessarily necessary for the physics component to push information to the Entity as it updates, 
    //    moreso for it to hold and manage the physics information for the entity while providing functions that allow
    //    the entity to query their physics component for desired information.
    void update(float fTimeInSeconds); // Overloading Parent's virtual update function
    void AIComponent::getCurrentAction(HovercraftEntity *mPlayer, HovercraftEntity *bot, float delta_time, Action *a);
    // Various initialization functions as needed.
    // this function will allow Entities to retrieve the Transform Matrix required to modify their mesh.
private:
    Action frames[MUTATION_SET][LOOK_AHEAD_FRAMES];
    int currentBest = 0;
    float currentBestEval = 0;
    int currentPlace = 0;
    int iEntityID;
    glm::vec3 seekPoint = vec3(200, 0, 30);
    int iComponentID;
    float timeChased = 0;
    vector<uvec2> path;
    int currentState = 0;
    vec2 seekLocation;
    int LastIndex = -1;
    bool nextPosMove = false;
    vec3 get2ndNearestSeekPoint(vec2 currentPos);
    vec3 getNearestSeekPoint(vec2 currentPos);
};

