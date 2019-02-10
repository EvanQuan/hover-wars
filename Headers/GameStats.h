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
		HIT_PLAYER,
		PICKUP_POWERUP,
	};

	enum eRemoveScoreReason
	{
		HIT,
	};

	~GameStats();

	void initializeStats();

	void addScore(ePlayer player, eAddScoreReason reason);
	void removeScore(ePlayer player, eRemoveScoreReason reason);

	void pickup(ePlayer player, ePowerup powerup);

private:
	GameStats();
	static GameStats* m_pInstance;

	void setScore(ePlayer player, int score);
	void addKillstreak(ePlayer playerToAdd, ePlayer playerHit);
	void resetKillstreak(ePlayer playerToReset, ePlayer playerHit);

	void initializeScores();
	void initializeKillstreaks();


	// multimap<ePlayer, int> scores;
	// multimap<pair<ePlayer, ePlayer>, int> killstreaks;

	// Window reference
	GLFWwindow* m_pWindow;
};



