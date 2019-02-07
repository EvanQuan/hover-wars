#include "EntityComponentHeaders/EntityComponent.h"

// Default Constructor:
//		Requires an EntityID for the Entity that the component is a part of
//			and a ComponentID issued by the EntityManager.
EntityComponent::EntityComponent(int iEntityID, int iComponentID)
{
	m_iEntityID = iEntityID;
	m_iComponentID = iComponentID;
}

// Destructor
EntityComponent::~EntityComponent()
{

}
