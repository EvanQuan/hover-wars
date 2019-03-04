#pragma once
#include "stdafx.h"

/*
Stores and calculates all in-game stats.

Player:
    Score:
        Current score
        Change in score
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
        SCORE_CURRENT = 0,
        SCORE_CHANGE,
        SCORE_TOTAL,
        KILLS_TOTAL,
        KILLS_TOTAL_AGAINST_PLAYER_1,
        KILLS_TOTAL_AGAINST_PLAYER_2,
        KILLS_TOTAL_AGAINST_PLAYER_3,
        KILLS_TOTAL_AGAINST_PLAYER_4,
        KILLS_TOTAL_AGAINST_BOT_1,
        KILLS_TOTAL_AGAINST_BOT_2,
        KILLS_TOTAL_AGAINST_BOT_3,
        KILLS_TOTAL_AGAINST_BOT_4,
        KILLS_TOTAL_AGAINST_BOTS,
        IS_DOMINATING_PLAYER_1,
        IS_DOMINATING_PLAYER_2,
        IS_DOMINATING_PLAYER_3,
        IS_DOMINATING_PLAYER_4,
        KILLSTREAK_CURRENT,
        KILLSTREAK_CURRENT_AGAINST_PLAYER_1,
        KILLSTREAK_CURRENT_AGAINST_PLAYER_2,
        KILLSTREAK_CURRENT_AGAINST_PLAYER_3,
        KILLSTREAK_CURRENT_AGAINST_PLAYER_4,
        KILLSTREAK_CURRENT_AGAINST_BOT_1,
        KILLSTREAK_CURRENT_AGAINST_BOT_2,
        KILLSTREAK_CURRENT_AGAINST_BOT_3,
        KILLSTREAK_CURRENT_AGAINST_BOT_4,
        KILLSTREAK_LARGEST,
        KILLSTREAK_LARGEST_AGAINST_PLAYER_1,
        KILLSTREAK_LARGEST_AGAINST_PLAYER_2,
        KILLSTREAK_LARGEST_AGAINST_PLAYER_3,
        KILLSTREAK_LARGEST_AGAINST_PLAYER_4,
        KILLSTREAK_LARGEST_AGAINST_BOT_1,
        KILLSTREAK_LARGEST_AGAINST_BOT_2,
        KILLSTREAK_LARGEST_AGAINST_BOT_3,
        KILLSTREAK_LARGEST_AGAINST_BOT_4,
        POWERUPS_TOTAL_PICKED_UP,
        ABILITIES_TOTAL_USED,
        ABILITY_ROCKET_USED,
        ABILITY_SPIKES_USED,
        ABILITY_TRAIL_USED,
        ABILITY_DASH_USED,
        STAT_COUNT,
    };

    enum eAddScoreReason
    {
        HIT_PLAYER_1 = 0,
        HIT_PLAYER_2,
        HIT_PLAYER_3,
        HIT_PLAYER_4,
        HIT_BOT_1,
        HIT_BOT_2,
        HIT_BOT_3,
        HIT_BOT_4,
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

    // Stats
    int get(eBot bot, eStat stat);
    void addScore(eBot bot, eAddScoreReason reason);

    // Cooldowns
    float get(eBot bot, eCooldown cooldown);
    void useAbility(eBot bot, eAbility ability);
    bool isOnCooldown(eBot bot, eCooldown cooldown);

    // Not sure if this should be public or private
    // Stats
    int get(eHovercraft hovercraft, eStat stat);
    void addScore(eHovercraft hovercraft, eAddScoreReason reason);

    // Cooldowns
    float get(eHovercraft hovercraft, eCooldown cooldown);
    void useAbility(eHovercraft hovercraft, eAbility ability);
    bool isOnCooldown(eHovercraft hovercraft, eCooldown cooldown);

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
    int stats[MAX_HOVERCRAFT_COUNT][STAT_COUNT];

    float cooldowns[MAX_HOVERCRAFT_COUNT][COOLDOWN_COUNT];

    // Actions
    // void hitBot(eHovercraft attacker, eBot hit);
    void hit(eHovercraft attacker, eHovercraft hit);
    // Score
    void updateAttackerAndHitScore(eHovercraft attacker, eHovercraft hit);
    int getScoreGainedForAttacker(eHovercraft attacker, eHovercraft hit);
    int getScoreLostForHit(eHovercraft attacker, eHovercraft hit);
    void addScore(eHovercraft attacker, int points);
    void removeScore(eHovercraft hit, int points);

    // Kills
    void updateAttackerAndHitKills(eHovercraft attacker, eHovercraft hit);


    // Killstreaks
    void updateAttackerAndHitKillstreak(eHovercraft attacker, eHovercraft hit);
    void addKillstreak(eHovercraft attacker, eHovercraft hit);
    void increaseCurrentTotalKillstreak(eHovercraft attacker);
    int getCurrentKillstreakAgainst(eHovercraft attacker, eHovercraft hit);
    void updateLargestTotalKillstreak(eHovercraft hovercraft);
    void resetKillstreak(eHovercraft attacker, eHovercraft hit);

    // Domination
    bool isDominating(eHovercraft attacker, eHovercraft hit);
    bool canStartDomination(eHovercraft attacker, eHovercraft hit);
    void dominate(eHovercraft attacker, eHovercraft hit);

    // Revenge
    void revenge(eHovercraft hovercraftToGetRevenge, eHovercraft hovercraftWasDominating);

    // Powerups
    void pickupPowerup(eHovercraft hovercraft);
    void addPowerupCount(eHovercraft hovercraft);

    void debug(eHovercraft hovercraft);

    unordered_map<eAddScoreReason, ePlayer> scoreReasonToPlayer = 
    {
        {HIT_PLAYER_1, PLAYER_1},
        {HIT_PLAYER_2, PLAYER_2},
        {HIT_PLAYER_3, PLAYER_3},
        {HIT_PLAYER_4, PLAYER_4},
    };

    unordered_map<eAddScoreReason, eBot> scoreReasonToBot = 
    {
        {HIT_BOT_1, BOT_1},
        {HIT_BOT_2, BOT_2},
        {HIT_BOT_3, BOT_3},
        {HIT_BOT_4, BOT_4},
    };

    unordered_map<eAddScoreReason, eHovercraft> scoreReasonToHovercraft = 
    {
        {HIT_PLAYER_1,  HOVERCRAFT_PLAYER_1},
        {HIT_PLAYER_2,  HOVERCRAFT_PLAYER_2},
        {HIT_PLAYER_3,  HOVERCRAFT_PLAYER_3},
        {HIT_PLAYER_4,  HOVERCRAFT_PLAYER_4},
        {HIT_BOT_1,     HOVERCRAFT_BOT_1},
        {HIT_BOT_2,     HOVERCRAFT_BOT_2},
        {HIT_BOT_3,     HOVERCRAFT_BOT_3},
        {HIT_BOT_4,     HOVERCRAFT_BOT_4},
    };
};



