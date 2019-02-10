#pragma once
#include "stdafx.h"

#define DOMINATION_COUNT 3
#define POINTS_GAINED_HIT_BOT 10
#define POINTS_GAINED_HIT_PLAYER 50
#define POINTS_GAINED_HIT_REVENGE 150
#define POINTS_GAINED_PER_KILLSTREAK 20
#define POINTS_GAINED_PICKUP_POWERUP 10
#define POINTS_LOST_GOT_HIT 10
#define POINTS_LOST_PER_KILLSTREAK 10

// Stat indices

/*
Stores and calculates all in-game stats.

Player:
	Score:
		Current score
		Total score
	Kills:
		Total kills
		Total kills against players
		Total kills against each player
		Total kills against bots
	Dominations:
		Current dominations between players
	Killstreaks:
		Current total killstreak
		Current killstreak between players
		Largest total killstreak
		Largest total killstreak between players
	Powerups:
		Total powerups picked up

Map:

Gets values from EntityManager to update stats as the game progresses.
Gives information to UserInterface to display the correct values.
*/
class GameStats
{
public:
	static GameStats* getInstance();

	/*
	NOTE: Do not use STAT_COUNT as a Stat value. It denotes the number of stats.
	*/
	enum eStat
	{
		CURRENT_SCORE,
		TOTAL_SCORE,
		TOTAL_KILLS,
		TOTAL_KILLS_AGAINST_PLAYER_1,
		TOTAL_KILLS_AGAINST_PLAYER_2,
		TOTAL_KILLS_AGAINST_PLAYER_3,
		TOTAL_KILLS_AGAINST_PLAYER_4,
		TOTAL_KILLS_AGAINST_BOTS,
		IS_DOMINATING_PLAYER_1,
		IS_DOMINATING_PLAYER_2,
		IS_DOMINATING_PLAYER_3,
		IS_DOMINATING_PLAYER_4,
		CURRENT_TOTAL_KILLSTREAK,
		CURRENT_KILLSTREAK_AGAINST_PLAYER_1,
		CURRENT_KILLSTREAK_AGAINST_PLAYER_2,
		CURRENT_KILLSTREAK_AGAINST_PLAYER_3,
		CURRENT_KILLSTREAK_AGAINST_PLAYER_4,
		LARGEST_TOTAL_KILLSTREAK,
		LARGEST_TOTAL_KILLSTREAK_AGAINST_PLAYER_1,
		LARGEST_TOTAL_KILLSTREAK_AGAINST_PLAYER_2,
		LARGEST_TOTAL_KILLSTREAK_AGAINST_PLAYER_3,
		LARGEST_TOTAL_KILLSTREAK_AGAINST_PLAYER_4,
		TOTAL_POWERUPS_PICKED_UP,
		STAT_COUNT,
	};

	enum eAddScoreReason
	{
		HIT_BOT,
		HIT_PLAYER_1,
		HIT_PLAYER_2,
		HIT_PLAYER_3,
		HIT_PLAYER_4,
		PICKUP_POWERUP,
	};


	enum eRemoveScoreReason
	{
		HIT,
	};

	~GameStats();

	void initializeStats();

	void addScore(ePlayer player, eAddScoreReason reason);
	int getScore(ePlayer player);

	void setPowerupStatus(ePlayer player, ePowerup powerup);
	ePowerup getPowerupStatus(ePlayer player);

	/*
	Overall game stats

	Everything is stored in a singular 2D int array for setting and querying
	efficiency.

	This is public so other classes, such as UserInterface may retrive values
	from it. Use the ePlayer first index, and eStat as the second.
	For example:

		int results = stats[PLAYER_1][CURRENT_KILLSTREAKS_AGAINST_PLAYER_2];

	will retrieve Player 1's current killstreaks against player 2.

	Please do not alter these values externally from other classes.
	*/
	int stats[MAX_PLAYER_COUNT][STAT_COUNT];

private:
	GameStats();
	static GameStats* m_pInstance;

	void hitBot(ePlayer playerAttacker);
	void hitPlayer(ePlayer playerAttacker, ePlayer playerHit);
	void updateAttackerAndHitScore(ePlayer playerAttacker, ePlayer playerHit);
	int getScoreForAttacker(ePlayer playerAttacker, ePlayer playerHit);
	int getScoreForHit(ePlayer playerAttacker, ePlayer playerHit);
	void addDomination(ePlayer playerAttacker, ePlayer playerHit);
	void resetDomination(ePlayer playerAttacker, ePlayer playerHit);
	bool isDominating(ePlayer playerAttacker, ePlayer playerHit);

	void initializePowerUpStatus();

	// Current game stats
	Bag<ePlayer> scores;
	Bag<ePlayer> killstreaks;
	Bag<pair<ePlayer, ePlayer>> dominations;
	unordered_map<ePlayer, ePowerup> powerupStatus;

	unordered_map<eAddScoreReason, ePlayer> scoreReasonToPlayer = 
	{
		{HIT_PLAYER_1, PLAYER_1},
		{HIT_PLAYER_2, PLAYER_2},
		{HIT_PLAYER_3, PLAYER_3},
		{HIT_PLAYER_4, PLAYER_4},
	};
};



