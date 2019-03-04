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
                              unsigned int iStatsID)
{
HovercraftEntity::initialize(sFileName, pObjectProperties, sShaderType, fScale);
    m_iStatsID = iStatsID;
    // Queue new position and add to total

}

/*
    Tells the HovercraftEntity that they were damaged. This is where the Hovercraft Entity will handle its "death" logic and award points to the winner.

    @param  eHitByType      The Entity Type that this Entity was hit by. This entity will either be a bot or a Player

    @TODO   This seems very general and may be able to be reworked with a better design.
*/
void PlayerEntity::hit(eEntityTypes eHitByType, unsigned int iNumber)
{
    // cout << "Player " << iNumber << " hit by " << eHitByType << endl;
    // Switch based on who hit the player
    switch (eHitByType)
    {
    case BOT_ENTITY:    // Hitting Entity was a bot, meaning that the bot #iNumber should get points for hitting this player #m_ePlayerID
        if (!isInvincible())
        {
            m_pGmStats->addScore(static_cast<eBot>(iNumber), static_cast<GameStats::eAddScoreReason>(m_iStatsID));
        }
        break;
        setInvincible();
    case PLAYER_ENTITY: // Hitting Entity was another player, meaning that the player #iNumber should get points for hitting this player #m_ePlayerID
        if (!isInvincible())
        {
            m_pGmStats->addScore(static_cast<ePlayer>(iNumber), static_cast<GameStats::eAddScoreReason>(m_iStatsID));
        }
        break;
        setInvincible();
    }
}
