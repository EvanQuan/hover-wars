#include "EntityHeaders/Entity.h"

// Default Constructor
Entity::Entity(int iID, vec3 vPosition, eEntityTypes eType)
{
    m_iID = iID;
    m_vPosition = vPosition;
    m_eType = eType;
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
