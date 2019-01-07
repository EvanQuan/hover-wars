#include "Entity.h"

// Default Constructor
Entity::Entity(int iID, vec3 vPosition)
{
	m_iID = iID;
	m_vPosition = vPosition;
}

// Destructor
Entity::~Entity()
{
	// Nothing to destruct
}