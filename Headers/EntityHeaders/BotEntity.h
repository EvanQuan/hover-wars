#pragma once
#include "EntityHeaders/HovercraftEntity.h"

class BotEntity :
    public HovercraftEntity
{
public:
    BotEntity(int iID, const vec3* vPosition);
    virtual ~BotEntity();

    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale,
                    eBot botID);
private:

    /*
    This ID is used for communicating with GameStats
    */
    eBot m_eBotID; 
};

