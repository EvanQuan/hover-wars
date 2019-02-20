#pragma once
#include "EntityHeaders/HovercraftEntity.h"

class BotEntity :
    public HovercraftEntity
{
public:
    BotEntity(int iID, const vec3* vPosition);
    virtual ~BotEntity();

private:
};

