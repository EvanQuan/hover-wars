#pragma once
#include "EntityComponentHeaders/EntityComponent.h"
#include <vector>
/*
For collisions
*/
#define LOOK_AHEAD_FRAMES 30
/**************************************************************
 * Name: AIComponent
 * Written by: Austin Eaton
\***************************************************************************************/
struct Action {
    bool actionsToTake[3] = { false,false,false };
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
    vector<Action *> frames;
    int iEntityID;
    int iComponentID;
};

