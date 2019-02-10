#include "GameStats.h"


// Singleton instance
GameStats* GameStats::m_pInstance = nullptr;

GameStats::GameStats()
{
	initializeStats();
}

GameStats* GameStats::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new GameStats();
	}
	return m_pInstance;
}

GameStats::~GameStats()
{
}

/*
Initialize all stats. This should be done at the start of every game,
or if the game resets.
*/
void GameStats::initializeStats()
{
	scores.clear();
	killstreaks.clear();
	initializePowerUpStatus();
}

/*
Add score to the specified player for the specified reason.
*/
void GameStats::addScore(ePlayer player, eAddScoreReason reason)
{

}

/*
@return the score of the specified player
*/
int GameStats::getScore(ePlayer player)
{
	return scores.count(player);
}

/*
Signifies a specified player has picked up the specified powerup.
*/
void GameStats::setPowerupStatus(ePlayer player, ePowerup powerup)
{
	powerupStatus.insert(make_pair(player, powerup));
}

/*
@return the power up status of the specified player
*/
ePowerup GameStats::getPowerupStatus(ePlayer player)
{
	return powerupStatus.at(player);
}


/*
Add to a player's killstreak against another player.
*/
void GameStats::addKillstreak(ePlayer playerToAdd, ePlayer playerHit)
{
	killstreaks.add(make_pair(playerToAdd, playerHit), 1);
}

/*
Reset a player's killstreak against another player.
*/
void GameStats::resetKillstreak(ePlayer playerToReset, ePlayer playerHit)
{
	killstreaks.erase(make_pair(playerToReset, playerHit));
}

/*
Check whether playerToCheck has a large enough killstreak against playerHit to
count as dominating.
@return true if playerToCheck is dominating playerHit
*/
bool GameStats::isDominating(ePlayer playerToCheck, ePlayer playerHit)
{
	return killstreaks.count(make_pair(playerToCheck, playerHit)) >= DOMINATION_COUNT;
}

/*
Initialize all players to not have any power ups activated.
*/
void GameStats::initializePowerUpStatus()
{
	for (int player = PLAYER_1; player <= PLAYER_4; player++)
	{
		powerupStatus.insert(make_pair((ePlayer)player, POWERUP_NONE));
	}
}
