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

    /*
    Track ability usage for players
    */
    bool useAbility(eAbility ability)
    {
        if (HovercraftEntity::useAbility(ability))
        {
            GAME_STATS->useAbility(m_ePlayerID, ability);
            return true;
        }
        return false;

    }

private:

    /*
    This ID is used for communicating with GameStats
    */
    ePlayer m_ePlayerID; 
};

