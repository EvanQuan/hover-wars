#pragma once
#include "EntityHeaders/HovercraftEntity.h"

class PlayerEntity :
    public HovercraftEntity
{
public:
    PlayerEntity(int iID, const vec3* vPosition);
    virtual ~PlayerEntity();

private:

};

