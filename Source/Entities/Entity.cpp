#include "EntityHeaders/Entity.h"

// Default Constructor
Entity::Entity(int iID, vec3 vPosition)
{
	m_iID = iID;
	m_vPosition = vPosition;
}

// Copy Constructor
Entity::Entity(const Entity& pCopy)
{
	m_iID = pCopy.m_iID;
	m_vPosition = pCopy.m_vPosition;
}

// Destructor
Entity::~Entity()
{
	// Nothing to destruct
}