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

/*
    Represents an accumulation of all actions a given AI should perform for a
    given frame. Involves both movement and ability usage.
*/
struct Action {
    // Signifies AI should fire a rocket for the next update
    bool shouldFireRocket = false;
    // Determines the extend and angle at which the hovercraft is to turn
    // -1: turn left, 0: neutral, 1: turn right
    float turn = 0.0f;
    // Fowards and backwards movement
    // -1: backwards, 0: neutral, 1: forwards
    float moveY = 0.0f;
    // Sideways movement
    // -1: left, 0: neutral, 1: right
    float moveX = 0.0f;
    // Signifies the AI should activate the flame trail for the next update
    bool shouldActivateTrail = false;
    // Signifies the AI should activate spikes for the next update
    // @TODO implement
    bool shouldActivateSpikes = false;
};
class AIComponent final :
    public EntityComponent
{
public:
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
    /*
    */
    enum eState {
        STATE_CHASE,
        STATE_SEEK
    };
    Action frames[MUTATION_SET][LOOK_AHEAD_FRAMES];
    int currentBest = 0;
    float currentBestEval = 0;
    int currentPlace = 0;
    int iEntityID;
    glm::vec3 seekPoint = vec3(200, 0, 30);
    int iComponentID;
    float timeChased = 0;
    vector<uvec2> path;
    /*
        Represents the current state of the AI. The state determines the mode
        of decision-making behaviour.
    */
    eState currentState = STATE_CHASE;

    vec2 seekLocation;
    int LastIndex = -1;
    bool nextPosMove = false;
    vec3 get2ndNearestSeekPoint(vec2 currentPos);
    vec3 getNearestSeekPoint(vec2 currentPos);
};

