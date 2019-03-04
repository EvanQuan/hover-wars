#include "EntityHeaders/PlayerEntity.h"

PlayerEntity::PlayerEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition, PLAYER_ENTITY)
{
}

PlayerEntity::~PlayerEntity()
{

}

void PlayerEntity::initialize(const string& sFileName,
                              const ObjectInfo* pObjectProperties,
                              const string& sShaderType,
                              float fScale,
                              ePlayer playerID)
{
    HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
    m_ePlayerID = playerID;
    // Queue new position and add to total

}
