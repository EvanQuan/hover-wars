#pragma once
#include "EntityComponentHeaders/EntityComponent.h"

/**************************************************************
 * Name: PhysicsComponent
 * Written by: James Cote and Austin Eaton
 * Description: Provides Physics functionality for Entities that require it.
 *		Any Entity that requires Physics adds a PhysicsComponent to their class
 *		and initializes it through the Entity Manager on their initialize function.
\***************************************************************************************/
class PhysicsComponent :
	public EntityComponent
{
public:
	// Constructor/Destructor - Destructor must be virtual for proper deletion through unique_ptrs
	PhysicsComponent();
	virtual ~PhysicsComponent();

	void update();


};

