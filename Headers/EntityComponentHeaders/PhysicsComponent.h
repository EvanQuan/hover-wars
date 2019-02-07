#pragma once
#include "EntityComponentHeaders/EntityComponent.h"
#include "Mesh.h"

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
	PhysicsComponent(int iEntityID, int iComponentID);
	virtual ~PhysicsComponent();

	// After Initialization, the update function should handle things for the Physics of this Entity.
	//	It's not necessarily necessary for the physics component to push information to the Entity as it updates, 
	//	moreso for it to hold and manage the physics information for the entity while providing functions that allow
	//	the entity to query their physics component for desired information.
	void update(duration<float> fTimeDelta); // Overloading Parent's virtual update function

	// Various initialization functions as needed.
	void initializeComponent(bool bStatic, Mesh const* pMeshReference /*Potentially required. Add any getter functions to the Mesh class as needed,
														 but make sure they're const functions as physics manager is not intended to modify the mesh. Maybe it needs to?*/);
	/*void initializeAsSpherical(); Maybe this style is required for different initialization types based on type of bounding mesh desired?*/

	// this function will allow Entities to retrieve the Transform Matrix required to modify their mesh.
	void getTransformMatrix(mat4* pReturnTransformMatrix);

private:
	bool m_bStatic;						// Flag for determining if the Physics Component is Static or Dynamic, I assume this is important and will influence how the Physics component functions under the hood.
	PhysicsManager* m_pPhysicsManager;	// Reference to Physics Manager for calling for any updates necessary.
	mat4 m_pTransformationMatrix;		// Stored Locally, maybe pulled from PhysicsManager on update?
};

