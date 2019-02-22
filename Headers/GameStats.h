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
    Multikills: ? This requires some notion of time ?
        Current multikills TODO
        Largest multikills TODO
    Killstreaks:
        Current total killstreak
        Current killstreak between players
        Largest total killstreak
        Largest total killstreak between players
    Powerups:
        Total powerups picked up
    Abilities:
        Total abilities used
        Count of each ability used

Game:
    

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
        CURRENT_SCORE = 0,
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
        TOTAL_ABILITIES_USED,
        ABILITY_ROCKET_USED,
        ABILITY_SPIKES_USED,
        ABILITY_TRAIL_USED,
        ABILITY_DASH_USED,
        STAT_COUNT,
    };

    enum eAddScoreReason
    {
        HIT_BOT = 0,
        HIT_PLAYER_1,
        HIT_PLAYER_2,
        HIT_PLAYER_3,
        HIT_PLAYER_4,
        PICKUP_POWERUP,
    };


    enum eRemoveScoreReason
    {
        HIT = 0,
    };

    enum eCooldown
    {
        COOLDOWN_ROCKET = 0,
        COOLDOWN_SPIKES,
        COOLDOWN_TRAIL,
        COOLDOWN_DASH,
        COOLDOWN_COUNT,
    };

    ~GameStats();

    void update(float fSecondsSinceLastUpdate);

    // Stats
    int get(ePlayer player, eStat stat);
    void addScore(ePlayer player, eAddScoreReason reason);

    // Cooldowns
    float get(ePlayer player, eCooldown cooldown);
    void useAbility(ePlayer player, eAbility ability);
    bool isOnCooldown(ePlayer player, eCooldown cooldown);

    void initialize();

private:
    GameStats();
    static GameStats* m_pInstance;

    void initializeStats();
    void initializeCooldowns();


    /*
    Overall game stats

    Everything is stored in a singular 2D int array for setting and querying
    efficiency.
    */
    int stats[MAX_PLAYER_COUNT][STAT_COUNT];

    float cooldowns[MAX_PLAYER_COUNT][COOLDOWN_COUNT];

    // Actions
    void hitBot(ePlayer playerAttacker);
    void hitPlayer(ePlayer playerAttacker, ePlayer playerHit);
    // Score
    void updateAttackerAndHitScore(ePlayer playerAttacker, ePlayer playerHit);
    int getScoreGainedForAttacker(ePlayer playerAttacker, ePlayer playerHit);
    int getScoreLostForHit(ePlayer playerAttacker, ePlayer playerHit);
    void addScore(ePlayer playerAttacker, int points);
    // Killstreaks
    void updateAttackerAndHitKillstreak(ePlayer playerAttacker, ePlayer playerHit);
    void addKillstreak(ePlayer playerAttacker, ePlayer playerHit);
    void resetKillstreak(ePlayer playerAttacker, ePlayer playerHit);
    bool isDominating(ePlayer playerAttacker, ePlayer playerHit);
    // Powerups
    void pickupPowerup(ePlayer player);
    void addPowerupCount(ePlayer player);

    unordered_map<eAddScoreReason, ePlayer> scoreReasonToPlayer = 
    {
        {HIT_PLAYER_1, PLAYER_1},
        {HIT_PLAYER_2, PLAYER_2},
        {HIT_PLAYER_3, PLAYER_3},
        {HIT_PLAYER_4, PLAYER_4},
    };
};



