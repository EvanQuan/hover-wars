#include "EntityHeaders/BotEntity.h"
#include "EntityManager.h"
#include "Enums/eCooldown.h"


/*
@Deprecated This will be deleted once all AI related calculated are moved
outside of this class.
*/
BotEntity::BotEntity(int iID, const vec3* vPosition)
    : HovercraftEntity(iID, vPosition)
{

}

BotEntity::~BotEntity()
{

}

void BotEntity::update(float fTimeInSeconds)
{
}
void BotEntity::initialize(const string& sFileName,
    const ObjectInfo* pObjectProperties,
    const string& sShaderType,
    float fScale)
{
    HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
}
