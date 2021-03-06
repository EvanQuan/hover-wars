#include "EntityHeaders/Entity.h"

// Default Constructor
Entity::Entity(int iID, vec3 vPosition, eEntityType eType)
{
    m_iID = iID;
    m_vPosition = vPosition;
    m_eType = eType;
    m_sName = to_string(iID);
}

// Copy Constructor
Entity::Entity(const Entity& pCopy)
{
    m_iID = pCopy.m_iID;
    m_vPosition = pCopy.m_vPosition;
}

// void Entity::handleCollision(const Entity* pOther) const
void Entity::handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg)
{
    /*Default, not implemented*/
}

// Destructor
Entity::~Entity()
{
    // Nothing to destruct
}
