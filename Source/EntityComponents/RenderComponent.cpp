#include "EntityComponentHeaders/RenderComponent.h"

// Default Constructor:
//		Requires an EntityID for the Entity that the component is a part of
//			and a ComponentID issued by the EntityManager.
RenderComponent::RenderComponent(int iEntityID, int iComponentID)
	: EntityComponent( iEntityID, iComponentID )
{
	
}

// Destructor
RenderComponent::~RenderComponent()
{

}

// Overloaded Update Function
void RenderComponent::update(void)
{

}