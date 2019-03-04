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
                    unsigned int iStatsID);

    // Overridden function for handling a hit by an opposing entity
    void hit(eEntityTypes eHitByType, unsigned int iNumber);
    // void hit(eEntityTypes eHitByType, unsigned int iNumber) const;

    /*
    Track ability usage for players
     TODO Move to Hovecraft, since all hovercrafts have ability tracking
    */
    bool useAbility(eAbility ability)
    {
        if (HovercraftEntity::useAbility(ability))
        {
            GAME_STATS->useAbility(static_cast<ePlayer>(m_iStatsID), ability);
            return true;
        }
        return false;

    }

private:
};

