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
	dominations.clear();
	initializePowerUpStatus();
}

/*
Add score to the specified player for the specified reason.
*/
void GameStats::addScore(ePlayer player, eAddScoreReason reason)
{
	switch (reason)
	{
	case HIT_BOT:
		hitBot(player);
		break;
	case HIT_PLAYER_1:
	case HIT_PLAYER_2:
	case HIT_PLAYER_3:
	case HIT_PLAYER_4:
		hitPlayer(player, scoreReasonToPlayer.at(reason));
		break;
	case PICKUP_POWERUP:
		break;
	}
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
Signifies that playerAttacker hit a bot.
*/
void GameStats::hitBot(ePlayer playerAttacker)
{
	scores.add(playerAttacker, POINTS_HIT_BOT);
}
/*
Signifies that playerAttacker hit playerHit

Updates killstreaks and scores.
*/
void GameStats::hitPlayer(ePlayer playerAttacker, ePlayer playerHit)
{
	addDomination(playerAttacker, playerHit);
}

/*
Add to a playerAttacker's domination streak against playerHit.
playerHit's domination streak against playerAttacker ends.
Players cannot have a domination streak against themselves.
*/
void GameStats::addDomination(ePlayer playerAttacker, ePlayer playerHit)
{
	if (playerAttacker != playerHit)
	{
		dominations.add(make_pair(playerAttacker, playerHit), 1);
		resetDomination(playerHit, playerAttacker);
	}
}

/*
Reset a playerHit's domination streak against playerAttacker.
*/
void GameStats::resetDomination(ePlayer playerHit, ePlayer playerAttacker)
{
	dominations.erase(make_pair(playerHit, playerAttacker));
}

/*
Check whether playerToCheck has a large enough killstreak against playerHit to
count as dominating.
@return true if playerToCheck is dominating playerHit
*/
bool GameStats::isDominating(ePlayer playerToCheck, ePlayer playerHit)
{
	return dominations.count(make_pair(playerToCheck, playerHit)) >= DOMINATION_COUNT;
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
