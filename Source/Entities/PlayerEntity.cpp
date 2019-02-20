#include "EntityHeaders/PlayerEntity.h"

PlayerEntity::PlayerEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition, PLAYER_ENTITY)
{
}

PlayerEntity::~PlayerEntity()
{

}
