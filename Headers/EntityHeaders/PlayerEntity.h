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

    // TODO What purpose does this serve?
    // It is currently not doing anything.
    ePlayer m_ePlayerID; 
};

