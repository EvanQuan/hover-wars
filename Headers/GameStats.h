#pragma once
#include "stdafx.h"

#define DOMINATION_COUNT 3
#define POINTS_HIT_BOT 10
#define POINTS_HIT_PLAYER 50
#define POINTS_HIT_REVENGE 150
#define POINTS_PICKUP_POWERUP 10
#define POINTS_PER_KILLSTREAK 10

/*
Stores and calculates all in-game stats.

Player:
Score
Killstreak
Ability cooldowns ?
Active powerups ?

Map:
?

Gets values from EntityManager to update stats as the game progresses.
Gives information to UserInterface to display the correct values.
*/
class GameStats
{
public:
	static GameStats* getInstance();

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

	void pickup(ePlayer player, ePowerup powerup);

private:
	GameStats();
	static GameStats* m_pInstance;

	void addKillstreak(ePlayer playerToAdd, ePlayer playerHit);
	void resetKillstreak(ePlayer playerToReset, ePlayer playerHit);

	Bag<ePlayer> scores;
	Bag<pair<ePlayer, ePlayer>> killstreaks;
	unordered_map<ePlayer, eAbility> powerupStatus;
};



