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

void
