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
Initialize all stats. This should be done at the start of every game, or if the
game resets.
*/
void GameStats::initializeStats()
{
	for (int player = PLAYER_1; player < MAX_PLAYER_COUNT; player++)
	{
		for (int stat = 0; stat < STAT_COUNT; stat++)
		{
			stats[player][stat] = 0;
		}
	}
}

/*
Get a stat. For example:

	int killstreak = gameStats.get(PLAYER_1, CURRENT_KILLSTREAKS_AGAINST_PLAYER_2);

will retrieve Player 1's current killstreaks against player 2.
*/
int GameStats::get(ePlayer player, eStat stat)
{
	return stats[player][stat];
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
		pickupPowerup(player);
		break;
	}
}

/*
Track that the player used an ability.
*/
void GameStats::useAbility(ePlayer player, eAbility ability)
{
	switch (ability)
	{
	case ABILITY_ROCKET:
		stats[player][ABILITY_ROCKET_USED]++;
		break;
	case ABILITY_SPIKES:
		stats[player][ABILITY_SPIKES_USED]++;
		break;
	case ABILITY_TRAIL:
		stats[player][ABILITY_TRAIL_USED]++;
		break;
	case ABILITY_DASH_BACK:
	case ABILITY_DASH_FORWARD:
	case ABILITY_DASH_LEFT:
	case ABILITY_DASH_RIGHT:
		stats[player][ABILITY_DASH_USED]++;
		break;
	}
	stats[player][TOTAL_ABILITIES_USED]++;
}

/*
Signifies that playerAttacker hit a bot.
*/
void GameStats::hitBot(ePlayer playerAttacker)
{
	addScore(playerAttacker, POINTS_GAINED_HIT_BOT);
}
/*
Signifies that playerAttacker hit playerHit

Updates killstreaks and scores.
*/
void GameStats::hitPlayer(ePlayer playerAttacker, ePlayer playerHit)
{
	updateAttackerAndHitKillstreak(playerAttacker, playerHit);
	updateAttackerAndHitScore(playerAttacker, playerHit);
}

/*
Update the scores from the result of playerAttacker hitting playerHit
*/
void GameStats::updateAttackerAndHitScore(ePlayer playerAttacker, ePlayer playerHit)
{
	addScore(playerAttacker, getScoreGainedForAttacker(playerAttacker, playerHit));
	stats[playerHit][CURRENT_SCORE] -= getScoreLostForHit(playerAttacker, playerHit);
}

/*
Get the score for playerAtacker to gain if they hit playerHit
*/
int GameStats::getScoreGainedForAttacker(ePlayer playerAttacker, ePlayer playerHit)
{
	int basePoints = POINTS_GAINED_HIT_PLAYER;
	int killstreakBonus = POINTS_GAINED_PER_KILLSTREAK * stats[playerAttacker][CURRENT_TOTAL_KILLSTREAK];
	return basePoints + killstreakBonus;
}

/*
Get the score for playerHit to lose if hit by playerAttacker
playerHit cannot lose more points than they have.
*/
int GameStats::getScoreLostForHit(ePlayer playerAttacker, ePlayer playerHit)
{
	int basePoints = POINTS_LOST_GOT_HIT;
	int killstreakBonus = POINTS_LOST_PER_KILLSTREAK * stats[playerHit][CURRENT_TOTAL_KILLSTREAK];
	int totalPointsLost = basePoints + killstreakBonus;
	return stats[playerHit][CURRENT_SCORE] > totalPointsLost ? totalPointsLost : stats[playerHit][CURRENT_SCORE];
}

/*
Add to a player's score.
*/
void GameStats::addScore(ePlayer playerAttacker, int points)
{
	stats[playerAttacker][CURRENT_SCORE] += points;
	stats[playerAttacker][TOTAL_SCORE] += points;
}

/*
Update the killstreaks from the results of playerAttacker hitting playerHit
*/
void GameStats::updateAttackerAndHitKillstreak(ePlayer playerAttacker, ePlayer playerHit)
{
	addKillstreak(playerAttacker, playerHit);
	resetKillstreak(playerHit, playerAttacker);
}
/*
Add to a playerAttacker's  killstreak against playerHit.
playerHit's domination streak against playerAttacker ends.
NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 
*/
void GameStats::addKillstreak(ePlayer playerAttacker, ePlayer playerHit)
{
	int currentTotalKillstreak = ++stats[playerAttacker][CURRENT_TOTAL_KILLSTREAK];
	int currentKillstreakAgainstPlayer = ++stats[playerAttacker][CURRENT_KILLSTREAK_AGAINST_PLAYER_1 + playerHit];
	if (currentTotalKillstreak > stats[playerAttacker][LARGEST_TOTAL_KILLSTREAK])
	{
		stats[playerAttacker][LARGEST_TOTAL_KILLSTREAK] = currentTotalKillstreak;
	}
	if (currentKillstreakAgainstPlayer >= DOMINATION_COUNT)
	{
		stats[playerAttacker][IS_DOMINATING_PLAYER_1 + playerHit] = 1;
	}
}

/*
Reset a playerHit's killstreak against playerAttacker.
NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 
*/
void GameStats::resetKillstreak(ePlayer playerHit, ePlayer playerAttacker)
{
	stats[playerHit][CURRENT_TOTAL_KILLSTREAK + playerAttacker] = 0;
	stats[playerHit][CURRENT_KILLSTREAK_AGAINST_PLAYER_1 + playerAttacker] = 0;
	stats[playerHit][IS_DOMINATING_PLAYER_1 + playerAttacker] = 0;
}

/*
Check whether playerToCheck has a large enough killstreak against playerHit to
count as dominating.

NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 

@return true if playerToCheck is dominating playerHit
*/
bool GameStats::isDominating(ePlayer playerToCheck, ePlayer playerHit)
{
	return stats[playerToCheck][IS_DOMINATING_PLAYER_1 + playerHit];
}


/*
Denotes a player has picked up a power up
*/
void GameStats::pickupPowerup(ePlayer player)
{
	addScore(player, POINTS_GAINED_PICKUP_POWERUP);
	addPowerupCount(player);
}

/*
Add to the player's total power up count
*/
void GameStats::addPowerupCount(ePlayer player)
{
	stats[player][TOTAL_POWERUPS_PICKED_UP]++;
}
