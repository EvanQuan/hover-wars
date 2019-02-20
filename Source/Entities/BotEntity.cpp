#include "EntityHeaders/BotEntity.h"

BotEntity::BotEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition, BOT_ENTITY)
{

}

BotEntity::~BotEntity()
{

}
