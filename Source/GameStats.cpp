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
	initializeScores();
	initializeKillstreaks();
}


void addKillstreak(ePlayer player)
{
	
}

/*
Initialize the scores of all players to 0
*/
void GameStats::initializeScores()
{
	for (int player = PLAYER_1; player <= PLAYER_4; player++) {
		this->setScore((ePlayer) player, 0);
	}
}

/*
Initialize the killstreak of all players to 0
*/
void GameStats::initializeKillstreaks()
{
	for (int playerToAdd = PLAYER_1; playerToAdd <= PLAYER_4; playerToAdd++)
	{
		for (int playerHit = PLAYER_1; playerHit <= PLAYER_4; playerHit++)
		{
			this->resetKillstreak((ePlayer)playerToAdd, (ePlayer)playerHit);
		}
	}
}

/*
Set a specified player's score to the specified value.
*/
void GameStats::setScore(ePlayer player, int score)
{
	// scores.insert(pair<ePlayer, int>(player, score));
}

/*
Add to a player's killstreak against another player
*/
void GameStats::addKillstreak(ePlayer playerToAdd, ePlayer playerHit)
{
	// if (killstreaks.find()
	// killstreaks.insert(pair<pair<ePlayer, ePlayer>(playerToAdd, playerHit), 1>);
}

void GameStats::resetKillstreak(ePlayer playerToReset, ePlayer playerHit)
{
}
