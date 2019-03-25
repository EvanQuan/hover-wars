#pragma once
#include "stdafx.h"

struct Award
{
    string name;
    string description;
    int points;
    int statValue;
};
/*
    Represents the end game stats for a given hovercraft. This is necessary for
    determining the winner at the end of the game.
*/
struct EndGameStat
{
    // The hovercraft this state corresponds to.
    eHovercraft hovercraft;
    // The score at the end of the game before awards are awarded.
    int beforeAwardsScore;
    // The score after awards are awarded. This is the final score.
    int afterAwardsScore;
    // List of awards gained. Composed of the name of the award, description of
    // award the points it awards.
    vector<Award> awards;
};

class GameInterface;
class HovercraftEntity;
/*
Stores and calculates all in-game stats.

Player:
    Score:
        Current score
        Change in score
        Total score
    Kills:
        Total kills
        Total deaths
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
    Kills
        First blood

Gets values from EntityManager to update stats as the game progresses.
Gives information to UserInterface to display the correct values.
*/
class GameStats
{
public:
    static GameStats* getInstance(int iWidth, int iHeight);
    static GameStats* getInstance();

    /*
    NOTE: Do not use HOVERCRAFTSTAT_COUNT as a Stat value. It denotes the number of stats.
    */
    enum eHovercraftStat
    {
        SCORE_CURRENT = 0,
        SCORE_CHANGE,
        SCORE_TOTAL,
        DEATHS_TOTAL,
        KILLS_TOTAL,
        KILLS_TOTAL_AGAINST_PLAYER_1,
        KILLS_TOTAL_AGAINST_PLAYER_2,
        KILLS_TOTAL_AGAINST_PLAYER_3,
        KILLS_TOTAL_AGAINST_PLAYER_4,
        KILLS_TOTAL_AGAINST_BOT_1,
        KILLS_TOTAL_AGAINST_BOT_2,
        KILLS_TOTAL_AGAINST_BOT_3,
        KILLS_TOTAL_AGAINST_BOT_4,
        KILLS_TOTAL_AGAINST_PLAYERS,
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
        KILLS_WITH_ROCKET,
        KILLS_WITH_SPIKES,
        KILLS_WITH_TRAIL,
        HOVERCRAFTSTAT_COUNT,
    };

    /* TODO unused */
    enum eGlobalStat
    {
       SCORE_LARGEST,
       GLOBALSTAT_COUNT
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

    enum eCooldown
    {
        COOLDOWN_ROCKET = 0,
        COOLDOWN_SPIKES,
        COOLDOWN_TRAIL_ACTIVATE,
        COOLDOWN_DASH,
        COOLDOWN_COUNT,
    };

    /*
        End game stats have different index values than normal stats since they
        are variable in size.

        Every vector 
        Row major order
            0           1               2 
        0   Hovercraft  End game score  Final score
        1               Award #1        Award #1 bonus
        2               Award #2        Award #2 bonus
                        ...
        N               Award #N        Award #N bonus
    */
    enum eEndGameStatRow
    {
        ENDGAME_ROW_HOVERCRAFT = 0,
    };
    enum eEndGameStatColumn
    {
        ENDGAME_COLUMN_HOVERCRAFT_ID = 0,
        ENDGAME_COLUMN_AWARD_NAME    = 0,
        ENDGAME_COLUMN_SCORE         = 1,
    };

    ~GameStats();

    void update(float fSecondsSinceLastUpdate);

    // Stats
    int get(eHovercraft hovercraft, eHovercraftStat stat) const;
    void addScore(eHovercraft hovercraft, eAddScoreReason reason);
    void addScore(eHovercraft hovercraft, eAddScoreReason reason, eAbility ability);

    // Cooldowns
    float get(eHovercraft hovercraft, eCooldown cooldown) const;
    void useAbility(eHovercraft hovercraft, eAbility ability);
    bool isOnCooldown(eHovercraft hovercraft, eCooldown cooldown) const;

    void reinitialize(int playerCount, int botCount);

    eHovercraft getEHovercraft(int entityID) const { return FuncUtils::getOrDefault(entityIDToHovercraft, entityID, HOVERCRAFT_INVALID); }
    bool isBot(int entityID) const { return isBot(getEHovercraft(entityID)); };
    bool isPlayer(int entityID) const { return isPlayer(getEHovercraft(entityID)); };

    bool isBot(HovercraftEntity* hovercraft) const;
    bool isPlayer(HovercraftEntity* hovercraft) const;

    bool isBot(eHovercraft hovercraft) const;
    bool isPlayer(eHovercraft hovercraft) const;

    vector<EndGameStat> getEndGameStats();

    int get(eGlobalStat stat) const { return globalStats[stat]; };
    bool hasLargestScore(eHovercraft hovercraft);

private:
    GameStats(int iWidth, int iHeight);
    static GameStats* m_pInstance;

    void initializeStats();
    void initializeCooldowns();
    void correspondEntitiesToHovercrafts();

    int getLargestScore();
    /*
    End game stats and awards

    Sorted and variable in size, depending on in-game results.
    */
    vector<EndGameStat> endGameStats;
    void initializeEndGameStats();
    void sortByHighestScoreFirst();
    bool winnerSortFunction(EndGameStat left, EndGameStat right);
    vector<eHovercraft> getHovercraftsThatHaveHighest(eHovercraftStat stat);
    vector<eHovercraft> getHovercraftsThatHaveZero(eHovercraftStat stat);

    // Award name : function that determines who won the award
    void awardAwards();
    void awardHighestStat(eHovercraftStat stat, string name, string description, int points);
    void awardZeroStat(eHovercraftStat stat, string name, string description, int points);

    void awardToHovercrafts(eHovercraftStat stat, string name, string description, int points, vector<eHovercraft> winners);


    /*
    Overall game stats

    Everything is stored in a singular 2D int array for setting and querying
    efficiency.
    */
    int stats[MAX_HOVERCRAFT_COUNT][HOVERCRAFTSTAT_COUNT];

    int globalStats[GLOBALSTAT_COUNT];

    float cooldowns[MAX_HOVERCRAFT_COUNT][COOLDOWN_COUNT];

    // Tracks first blood
    bool firstBloodHappened;

    // Actions
    // void hitBot(eHovercraft attacker, eBot hit);
    void hit(eHovercraft attacker, eHovercraft hit);
    // Score
    void updateAttackerAndHitScore(eHovercraft attacker, eHovercraft hit);
    int getScoreGainedForAttacker(eHovercraft attacker, eHovercraft hit);
    int getScoreLostForHit(eHovercraft attacker, eHovercraft hit) const;
    void addScore(eHovercraft attacker, int points);
    void removeScore(eHovercraft hit, int points);

    // Kills
    void updateAttackerAndHitKills(eHovercraft attacker, eHovercraft hit);


    // Killstreaks
    void updateAttackerAndHitKillstreak(eHovercraft attacker, eHovercraft hit);
    void addKillstreak(eHovercraft attacker, eHovercraft hit);
    void increaseCurrentTotalKillstreak(eHovercraft attacker);
    int getCurrentKillstreakAgainst(eHovercraft attacker, eHovercraft hit) const;
    void updateLargestTotalKillstreak(eHovercraft hovercraft);
    void resetKillstreak(eHovercraft attacker, eHovercraft hit);

    // Domination
    bool isDominating(eHovercraft attacker, eHovercraft hit) const;
    bool canStartDomination(eHovercraft attacker, eHovercraft hit) const;
    void dominate(eHovercraft attacker, eHovercraft hit);

    // Revenge
    void revenge(eHovercraft hovercraftToGetRevenge, eHovercraft hovercraftWasDominating);

    // Powerups
    void pickupPowerup(eHovercraft hovercraft);
    void addPowerupCount(eHovercraft hovercraft);

    void debug(eHovercraft hovercraft);

    // @Deprecated, unused, due to perfect correspondance
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

    /*
        This corresponds the entityIDs to eHovercrafts.
        This will only be filled with values for hovercrafts that exist in the
        game.
    */
    unordered_map<int, eHovercraft> entityIDToHovercraft;

    GameInterface* m_pGameInterface;

    int m_iPlayerCount;
    int m_iBotCount;
};



