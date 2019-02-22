#pragma once
#include "EntityHeaders/HovercraftEntity.h"

class PlayerEntity :
    public HovercraftEntity
{
public:
    PlayerEntity(int iID, const vec3* vPosition);
    virtual ~PlayerEntity();

    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale,
                    ePlayer playerID);

private:

    /*
    This ID is used for communicating with GameStats
    */
    ePlayer m_ePlayerID; 
};

