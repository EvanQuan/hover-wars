#pragma once
#include "stdafx.h"

/************************************************************
* Name: EntityComponent
* Written by: James Cote
* Description: Base class for Entity Components that can be attached to Entities.
*        Each Entity Component implements an update function used to define unique tick
*        logic to execute as necessary. The EntityManager maintains all active Entity
*        Components as the base constructor registers itself with the manager.
***************************************************************/
class EntityComponent
{
public:
    virtual ~EntityComponent();

    virtual void update(duration<float> fTimeDelta) = 0;

    // Getters and Setters
    int getEntityID() { return m_iEntityID; }
    int getComponentID() { return m_iComponentID; }

protected: // Generally, Constructors only accessible by children constructors
    EntityComponent(int iEntityID, int iComponentID);
    EntityComponent(const EntityComponent* pCopy) {}
    EntityComponent& operator=(const EntityComponent& pRHS) {}

    // ID of the Entity that it's a component of
    //    and ID of this component assigned by the Entity Manager.
    int m_iEntityID, m_iComponentID;    
};
