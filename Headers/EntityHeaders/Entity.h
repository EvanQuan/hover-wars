#pragma once
#include "stdafx.h"

// Name: Entity
// Written by: James Cote
// Description: Base class for any interactable object in the game.
class Entity
{
public:
    Entity(int iID, vec3 vPosition, eEntityTypes eType);
    Entity(const Entity& pCopy);
    virtual ~Entity();

    virtual void update(float fTimeInMilliseconds) = 0;
    virtual void getSpatialDimensions(vec3* pNegativeCorner, vec3* pPositiveCorner) = 0;

    // Getters/Setters
    vec3 getPosition() { return m_vPosition; }
    int getID() { return m_iID; }
    eEntityTypes getType() { return m_eType; }

protected:
    vec3 m_vPosition;
    int m_iID;
    eEntityTypes m_eType;
};
