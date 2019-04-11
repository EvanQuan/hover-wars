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
    bool shouldActivateRocket = false;
    // Signifies the AI should activate the flame trail for the next update
    bool shouldActivateTrail = false;
    // Signifies the AI should activate spikes for the next update
    bool shouldActivateSpikes = false;
    // Determines the extend and angle at which the hovercraft is to turn
    // -1: turn left, 0: neutral, 1: turn right
    float turn = 0.0f;
    // Fowards and backwards movement
    // -1: backwards, 0: neutral, 1: forwards
    float moveY = 0.0f;
    // Sideways movement
    // -1: left, 0: neutral, 1: right
    float moveX = 0.0f;
};
class AIComponent final :
    public EntityComponent
{
public:
    // Constructor/Destructor - Destructor must be virtual for proper deletion through unique_ptrs
    AIComponent(int iEntityID, int iComponentID, unsigned int mapNumber);
    ~AIComponent();
    // void AIComponent::initalize(glm::vec3 playerPos, glm::vec3 playerVel, glm::vec3 botPosition, glm::vec3 botVel, float botRotation);
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
        Possible modes the AIComponent can be in.
    */
    enum eMode {
        // Wants to move towards the target hovercraft.
        // Path calculation is based on the target hovercraft.
        MODE_CHASE,
        // Wants to move towards the nearest seek point on the map
        // Path calculation is based on location of nearest seek points.
        MODE_SEEK,
        MODE_EVADE
    };
    vec2 seekPointsAI[8];
    vector<uvec2> getSeekPath();
    vector<uvec2> getChasePath() const;

    void updateBotAndTargetLocations(const HovercraftEntity *target, const HovercraftEntity *bot);
    void determinePath();
    void determineMode(float distanceToTarget,HovercraftEntity *bot);
    void determinePosition(HovercraftEntity *bot, const vec3 &botPosition, float fTimeInSeconds);

    bool shouldFireRocket(float accuracy);

    eMode modeSequence[10];
    int durations[10];
    int currentActionNum = 0;

    void determineTurn(const HovercraftEntity *bot, const vec3 &distanceVectorToTarget, const vec3 &botDirectionVector, Action *a);
    bool shouldChooseSeekMode(float distanceToTarget);

    bool shouldActivateRocket(const HovercraftEntity *bot, float accuracy);
    bool shouldActivateSpikes(const HovercraftEntity *bot, float distanceToTarget);
    bool shouldActivateTrail(const HovercraftEntity *bot);


    int currentBest = 0;
    float currentBestEval = 0;
    int currentPlace = 0;
    int iEntityID;
    glm::vec3 seekPoint = vec3(200, 0, 30);
    int iComponentID;
    float timeChased = 0;
    vector<uvec2> path;
    /*
        Represents the current mode of the AI. The mode determines which
        decision-making behaviour to act upon.
    */
    eMode m_eCurrentMode = MODE_CHASE;

    vec2 seekLocation;
    int lastIndex = -1;

    vec3 get2ndNearestSeekPoint(vec2 currentPos) const;
    vec3 getNearestSeekPoint(vec2 currentPos) const;

    void updateSeekPoint(const vec3 &botPos);
    SpatialDataMap *m_pSpatialDataMap;

    // Player location
    unsigned int minXTarget, minYTarget, maxXTarget, maxYTarget;
    // Bot location
    unsigned int minXBot, minYBot, maxXBot, maxYBot;

};

