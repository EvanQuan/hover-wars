#pragma once
#include "stdafx.h"

/*
Stores and calculates all in-game stats.

Player:
Score
Killstreak
Ability cooldowns ?
Active powerups ?

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
		HIT_PLAYER,
		HIT_REVENGE,
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



