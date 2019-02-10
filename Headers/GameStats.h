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
		Largest dominations between players
	Killstreaks:
		Current killstreak
		Largest killstreak
	Powerups:
		Total powerups picked up
		Total powerups picked up of each type

Map:

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
	int getScore(ePlayer player);

	void setPowerupStatus(ePlayer player, ePowerup powerup);
	ePowerup getPowerupStatus(ePlayer player);

private:
	GameStats();
	static GameStats* m_pInstance;

	void hitBot(ePlayer playerAttacker);
	void hitPlayer(ePlayer playerAttacker, ePlayer playerHit);
	void updateHitScore(ePlayer playerAttacker, ePlayer playerHit);
	void addDomination(ePlayer playerAttacker, ePlayer playerHit);
	void resetDomination(ePlayer playerAttacker, ePlayer playerHit);
	bool isDominating(ePlayer playerAttacker, ePlayer playerHit);

	void initializePowerUpStatus();

	Bag<ePlayer> scores;
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



