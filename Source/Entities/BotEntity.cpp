#include "EntityHeaders/BotEntity.h"

BotEntity::BotEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition, BOT_ENTITY)
{

}

BotEntity::~BotEntity()
{

}
void BotEntity::initialize(const string& sFileName,
    const ObjectInfo* pObjectProperties,
    const string& sShaderType,
    float fScale,
    eBot botID)
{
    HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
    m_eBotID = botID;
}
