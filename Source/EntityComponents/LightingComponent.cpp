#include "EntityComponentHeaders/LightingComponent.h"

// Default Constructor:
//		Requires an EntityID for the Entity that the component is a part of
//			and a ComponentID issued by the EntityManager.
LightingComponent::LightingComponent(int iEntityID, int iComponentID)
	: EntityComponent( iEntityID, iComponentID )
{
	
}

// Destructor
LightingComponent::~LightingComponent()
{

}

// Overloaded Update Function
void LightingComponent::update(double dTimeDelta)
{

}

// Initializes this Lighting Component as a Simple Point Light with a position and a color
void LightingComponent::initializeAsPointLight(const vec3* vPosition, const vec3* vColor)
{
	m_vPosition = *vPosition;
	m_vColor = *vColor;
}