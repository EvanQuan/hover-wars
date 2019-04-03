#pragma once
#include "stdafx.h"

// Name: Entity
// Written by: James Cote
// Description: Base class for any interactable object in the game.
class Entity
{
public:
    Entity(int iID, vec3 vPosition, eEntityType eType);
    Entity(const Entity& pCopy);
    virtual ~Entity();

    virtual void update(float fTimeInSeconds) = 0;
    virtual void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) const = 0;
    virtual void handleCollision(Entity* pOther, unsigned int iColliderMsg, unsigned int iVictimMsg);

    // Entit
    virtual void reinitialize() {}

    // Getters/Setters
    vec3 getPosition() const { return m_vPosition; }
    int getID() const { return m_iID; }
    const char* getName() const { return m_sName.c_str(); }
    eEntityType getType() const { return m_eType; }


protected:
    vec3 m_vPosition;
    int m_iID;
    string m_sName;
    eEntityType m_eType;
};
