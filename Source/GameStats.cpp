#include "GameStats.h"
#include "UserInterface/GameInterface.h"
#include "EntityManager.h"
#include "EntityHeaders/HovercraftEntity.h"

/*
Number of killstreaks against another player to count as domination
*/
#define DOMINATION_COUNT 3
/*
Base points gained for hitting a bot
*/
#define POINTS_GAINED_HIT_BOT 10
/*
Base points gained for hitting a player
*/
#define POINTS_GAINED_HIT_PLAYER 50
/*
Number of extra points gained when a player gets revenge
*/
#define POINTS_GAINED_HIT_REVENGE 100
/*
Number of extra points gained when a player gets the first kill
*/
#define POINTS_GAINED_FIRST_BLOOD 150
/*
Players gain additional points against other players based on their current
total killstreak. This gives players an incentive to not get hit.
*/
#define POINTS_GAINED_PER_KILLSTREAK 20

/*
Players gain additional points against other played based on that hit player's
current total killstreak. This gives players that have high killstreaks higher
priority targets to attack.
*/
#define POINTS_GAINED_PER_HIT_KILLSTREAK 10
/*
Base points for picking up a power up
*/
#define POINTS_GAINED_PICKUP_POWERUP 10
/*
Base points lost for getting hit
*/
#define POINTS_LOST_GOT_HIT 30
/*
Additional points lost per own killstreak. This makes having a large killstreak
risky as you will lose more points.
*/
#define POINTS_LOST_PER_KILLSTREAK 10
/*
Notifies a killstreak message once player hits a milestone.
*/
#define CURRENT_TOTAL_KILLSTREAK_MILESTONE 5


// Singleton instance
GameStats* GameStats::m_pInstance = nullptr;

GameStats::GameStats(int iWidth, int iHeight)
{
    m_pGameInterface = GameInterface::getInstance(iWidth, iHeight);
}

GameStats* GameStats::getInstance(int iWidth, int iHeight)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new GameStats(iWidth, iHeight);
    }
    return m_pInstance;
}

GameStats * GameStats::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

GameStats::~GameStats()
{
}

/*
In order to keep track of cooldowns, GameStats must be updated in sync with the
rest of the game.

This will decrease the cooldown value all all abilities by the time that has
passed.

@NOTE the cooldown values will become negative once they hit zero. When
factoring all cooldown ability checks and updates amongst the stats,
UserInterface, and hovercraft entities, it it cheaper this way.
This may change later in the future.
*/
void GameStats::update(float fSecondsSinceLastUpdate)
{
    for (int player = PLAYER_1; player < MAX_PLAYER_COUNT; player++)
    {
        for (int cooldown = 0; cooldown < COOLDOWN_COUNT; cooldown++)
        {
            cooldowns[player][cooldown] -= fSecondsSinceLastUpdate;
        }
    }
}

/*
Initialize all stats and cooldowns to 0.

This should be called at the start of every game, or if the game resets.
It should also be called AFTER players and bots have been initialized.
*/
void GameStats::reinitialize(int playerCount, int botCount)
{
    m_iPlayerCount = playerCount;
    m_iBotCount = botCount;

    initializeStats();
    initializeCooldowns();
    correspondEntitiesToHovercrafts();
    firstBloodHappened = false;
}

/*
Set all stats to 0
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
Set all cooldown timings to 0
*/
void GameStats::initializeCooldowns()
{
    for (int player = PLAYER_1; player < MAX_PLAYER_COUNT; player++)
    {
        for (int cooldown = 0; cooldown < COOLDOWN_COUNT; cooldown++)
        {
            cooldowns[player][cooldown] = 0.0f;
        }
    }
}

void GameStats::correspondEntitiesToHovercrafts()
{
    entityIDToHovercraft.clear();
    vector<HovercraftEntity*> players = ENTITY_MANAGER->m_pPlayerEntityList;
    vector<HovercraftEntity*> bots = ENTITY_MANAGER->m_pBotEntityList;
    cout << "Players:" << endl;
    int id;
    eHovercraft hovercraft;
    for (size_t i = 0, size = players.size(); i < size; ++i)
    {
        id = players.at(i)->getID();
        hovercraft = static_cast<eHovercraft>(i);
        entityIDToHovercraft.insert({id, hovercraft});
        cout << "ID: " << id << " | Hovercraft: " << hovercraft << endl;
    }
    cout << "Bots:" << endl;
    for (size_t i = 0, size = bots.size(); i < size; ++i)
    {
        id = bots.at(i)->getID();
        hovercraft = static_cast<eHovercraft>(i + HOVERCRAFT_BOT_1);
        entityIDToHovercraft.insert({id, hovercraft});
        cout << "ID: " << id << " | Hovercraft: " << hovercraft << endl;
    }
}

/*
Get a stat about a specified hovercraft.

@param hovercraft   to get stat about
@param stat         to retrieve
*/
int GameStats::get(eHovercraft hovercraft, eStat stat) const
{
    return stats[hovercraft][stat];
}

/*
Get a cooldown.
*/
float GameStats::get(eHovercraft hovercraft, eCooldown cooldown) const
{
    return cooldowns[hovercraft][cooldown];
}

/*
@return true is ability is ready to be used
*/
bool GameStats::isOnCooldown(eHovercraft hovercraft, eCooldown cooldown) const
{
    return cooldowns[hovercraft][cooldown] <= 0.0f;
}

/*
Add score to the specified player for the specified reason.
*/
void GameStats::addScore(eHovercraft hovercraft, eAddScoreReason reason)
{
    switch (reason)
    {
    case HIT_BOT_1:
    case HIT_BOT_2:
    case HIT_BOT_3:
    case HIT_BOT_4:
    case HIT_PLAYER_1:
    case HIT_PLAYER_2:
    case HIT_PLAYER_3:
    case HIT_PLAYER_4:
        hit(hovercraft, scoreReasonToHovercraft.at(reason));
        break;
    case PICKUP_POWERUP:
        pickupPowerup(hovercraft);
        break;
    }
}

/*
Track that the hovecraft used an ability.
This is used to track all the past abilities the hovercraft has used for that
round, which may be needed if post-round achievements are implemented.

@param hovercraft   that used ability
@param ability      used
*/
void GameStats::useAbility(eHovercraft hovercraft, eAbility ability)
{
    switch (ability)
    {
    case ABILITY_ROCKET:
        stats[hovercraft][ABILITY_ROCKET_USED]++;
        break;
    case ABILITY_SPIKES:
        stats[hovercraft][ABILITY_SPIKES_USED]++;
        break;
    case ABILITY_TRAIL_ACTIVATE:
        stats[hovercraft][ABILITY_TRAIL_USED]++;
        break;
    case ABILITY_DASH_BACK:
    case ABILITY_DASH_FORWARD:
    case ABILITY_DASH_LEFT:
    case ABILITY_DASH_RIGHT:
        stats[hovercraft][ABILITY_DASH_USED]++;
        break;
    }
    stats[hovercraft][ABILITIES_TOTAL_USED]++;
}

/*
Signifies that the attacker hovecraft hit the hit hovercraft
Upon getting hit, the attacker gains score, and the hit hovercraft loses score

Updates killstreaks and scores.
*/
void GameStats::hit(eHovercraft attacker, eHovercraft hit)
{
    // Update score first
    updateAttackerAndHitScore(attacker, hit);

    // Update kill statuses
    updateAttackerAndHitKills(attacker, hit);
    updateAttackerAndHitKillstreak(attacker, hit);

    m_pGameInterface->displayMessage(attacker, hit, GameInterface::KILL_MESSAGE_KILL);

#ifndef NDEBUG
    // cout << "Player " << attacker << " hit Player " << attacker << endl;
    // debug(attacker);
    // debug(hit);
#endif
}

void GameStats::debug(eHovercraft hovercraft)
{
    cout << "\tPlayer " << hovercraft << endl
        << "\t\tscore: " << stats[hovercraft][SCORE_CURRENT] << endl
        << "\t\ttotal kills: " << stats[hovercraft][KILLS_TOTAL] << endl
        << "\t\tcurrent total killstreak: " << stats[hovercraft][KILLSTREAK_CURRENT] << endl
        << "\t\tlargest total killstreak: " << stats[hovercraft][KILLSTREAK_LARGEST] << endl
        << "\t\tcurrent killstreak against Player 0: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_1) << endl
        << "\t\tcurrent killstreak against Player 1: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_2) << endl
        << "\t\tcurrent killstreak against Player 2: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_3) << endl
        << "\t\tcurrent killstreak against Player 3: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_4) << endl
        << "\t\tis dominating Player 0: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_1) << endl
        << "\t\tis dominating Player 1: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_2) << endl
        << "\t\tis dominating Player 2: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_3) << endl
        << "\t\tis dominating Player 3: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_4) << endl
        << "\t\ttotal abilities used: " << stats[hovercraft][ABILITIES_TOTAL_USED] << endl
        << "\t\ttotal rockets used: " << stats[hovercraft][ABILITY_ROCKET_USED] << endl
        << "\t\ttotal spikes used: " << stats[hovercraft][ABILITY_SPIKES_USED] << endl
        << "\t\ttotal trails used: " << stats[hovercraft][ABILITY_TRAIL_USED] << endl
        << "\t\ttotal dashes used: " << stats[hovercraft][ABILITY_DASH_USED] << endl;
}

/*
Update the scores from the result of attacker hitting hit

This ONLY updates the score values from the current kill statuses of the
attacker and hit.
*/
void GameStats::updateAttackerAndHitScore(eHovercraft attacker, eHovercraft hit)
{
    addScore(attacker, getScoreGainedForAttacker(attacker, hit));
    removeScore(hit, getScoreLostForHit(attacker, hit));
}

/*
Get the score for playerAtacker to gain if they hit hit
Checks for first blood.

@param attacker     to gain score
@param hit          that attacker hit
*/
int GameStats::getScoreGainedForAttacker(eHovercraft attacker, eHovercraft hit)
{
    int basePoints = FuncUtils::hovercraftToPlayer(attacker) != PLAYER_INVALID ?
        POINTS_GAINED_HIT_PLAYER : POINTS_GAINED_HIT_BOT;
    int killstreakBonus = POINTS_GAINED_PER_KILLSTREAK * stats[attacker][KILLSTREAK_CURRENT];
    int killstreakEndingBonus = POINTS_GAINED_PER_HIT_KILLSTREAK * stats[hit][KILLSTREAK_CURRENT];
    int revengeBonus = isDominating(hit, attacker) ? POINTS_GAINED_HIT_REVENGE : 0;
    int firstBloodBonus;
    if (firstBloodHappened) {
        firstBloodBonus = 0;
    } else {
        firstBloodBonus = POINTS_GAINED_FIRST_BLOOD;
        firstBloodHappened = true;
        m_pGameInterface->displayMessage(attacker, hit, GameInterface::eKillMessage::KILL_MESSAGE_FIRST_BLOOD);
    }
    return basePoints + killstreakBonus + revengeBonus + firstBloodBonus;
}

/*
Get the score for hit to lose if hit by attacker
hit cannot lose more points than they have.

@param attacker     that caused hit to lose score
@param hit          to lose score
*/
int GameStats::getScoreLostForHit(eHovercraft attacker, eHovercraft hit) const
{
    int basePoints = POINTS_LOST_GOT_HIT;
    int killstreakBonus = POINTS_LOST_PER_KILLSTREAK * stats[hit][KILLSTREAK_CURRENT];
    int totalPointsLost = basePoints + killstreakBonus;
    return stats[hit][SCORE_CURRENT] > totalPointsLost ? totalPointsLost : stats[hit][SCORE_CURRENT];
}

/*
Add to a player's score.
*/
void GameStats::addScore(eHovercraft attacker, int points)
{
    stats[attacker][SCORE_CHANGE] = points;
    stats[attacker][SCORE_CURRENT] += points;
    stats[attacker][SCORE_TOTAL] += points;
}

void GameStats::removeScore(eHovercraft hit, int points)
{
    stats[hit][SCORE_CHANGE] = -points;
    stats[hit][SCORE_CURRENT] -= points;
}


/*
Update the attacker's total kills, and total kills against hit player
*/
void GameStats::updateAttackerAndHitKills(eHovercraft attacker, eHovercraft hit)
{
    stats[attacker][KILLS_TOTAL]++;
    stats[attacker][KILLS_TOTAL_AGAINST_PLAYER_1 + hit]++;

    if (isBot(hit))
    {
        stats[attacker][KILLS_TOTAL_AGAINST_BOTS]++;
    }
    else if (isPlayer(hit))
    {
        stats[attacker][KILLS_TOTAL_AGAINST_PLAYERS]++;
    }
}

bool GameStats::isBot(eHovercraft hovercraft) const
{
    return HOVERCRAFT_BOT_1 <= hovercraft && hovercraft <= HOVERCRAFT_BOT_4;
}

bool GameStats::isPlayer(eHovercraft hovercraft) const
{
    return HOVERCRAFT_PLAYER_1 <= hovercraft && hovercraft <= HOVERCRAFT_PLAYER_4;
}

/*
Update the killstreaks from the results of attacker hitting hit
*/
void GameStats::updateAttackerAndHitKillstreak(eHovercraft attacker, eHovercraft hit)
{
    // Increase the attacker's killstreak
    addKillstreak(attacker, hit);
    // Reset the hit's killstreaks
    resetKillstreak(hit, attacker);
}
/*
Add to a attacker's killstreak against hit.
hit's domination streak against attacker ends.

This does not moditfy hit's data
*/
void GameStats::addKillstreak(eHovercraft attacker, eHovercraft hit)
{
    // Update attacker's current total killstreak
    increaseCurrentTotalKillstreak(attacker);
    // notify if attacker reached current total killstreak milestone
    int killstreak = stats[attacker][KILLSTREAK_CURRENT];
    if (killstreak > CURRENT_TOTAL_KILLSTREAK_MILESTONE)
    {
        m_pGameInterface->displayMessage(attacker, hit, GameInterface::eKillMessage::KILL_MESSAGE_KILLSTREAK);
    }

    // Update attacker's current total killstreak against hit
    stats[attacker][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + hit]++;

    // Update dominating if exceeds domination count and not already dominating
    if (canStartDomination(attacker, hit))
    {
        dominate(attacker, hit);
    }
}


/*
Increase the player's current total killstreak and check for largest total
killstreak updating.
*/
void GameStats::increaseCurrentTotalKillstreak(eHovercraft hovercraft)
{
    stats[hovercraft][KILLSTREAK_CURRENT]++;
    updateLargestTotalKillstreak(hovercraft);
}

/*

*/
void GameStats::updateLargestTotalKillstreak(eHovercraft hovercraft)
{
    if (stats[hovercraft][KILLSTREAK_CURRENT] > stats[hovercraft][KILLSTREAK_LARGEST])
    {
        stats[hovercraft][KILLSTREAK_LARGEST] = stats[hovercraft][KILLSTREAK_CURRENT];
    }
}

int GameStats::getCurrentKillstreakAgainst(eHovercraft attacker, eHovercraft hit) const
{
    return stats[attacker][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + hit];
}

/*
Reset a hit's killstreak against attacker.
May end a domination if exists.

@param hit      to reset killstreak to 0
@param attacker that ended hit's killstreak
*/
void GameStats::resetKillstreak(eHovercraft hit, eHovercraft attacker)
{
    // Reset current total killstreak
    stats[hit][KILLSTREAK_CURRENT] = 0;
    // Reset current total killstreak against attacker
    stats[hit][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + attacker] = 0;

    // If player hit was dominating attacker, disable domination and 
    // player revenge sound.
    if (isDominating(hit, attacker))
    {
        revenge(attacker, hit);
    }
}

/*
Check whether attacker has a large enough killstreak against hit to
count as dominating.

@return true if attacker is dominating hit
*/
bool GameStats::isDominating(eHovercraft attacker, eHovercraft hit) const
{
    return stats[attacker][IS_DOMINATING_PLAYER_1 + hit];
}

/*
attacker can start domination against playerHIt if
1. attacker's killstreak against hit is at least DOMINATION_COUNT
2. attacker is not already dominating hit

@param attacker     to check if can start domination on hit
@param hit          to check if can be dominated by attacker
*/
bool GameStats::canStartDomination(eHovercraft attacker, eHovercraft hit) const
{
    return getCurrentKillstreakAgainst(attacker, hit) >= DOMINATION_COUNT
        && !isDominating(attacker, hit);
}

/*
Enable attacker's domaination status against hit

@param attacker     to get domination
@param hit          to be dominated
*/
void GameStats::dominate(eHovercraft attacker, eHovercraft hit)
{
    // Ad hoc for single player
    m_pGameInterface->displayMessage(attacker, hit,
                                   GameInterface::eKillMessage::KILL_MESSAGE_DOMINATION);
    stats[attacker][IS_DOMINATING_PLAYER_1 + hit] = true;
}
/*
Disable hit's domination status against attacker.

@param attacker     to get revenge on hit
@param hit          was dominating attacker
*/
void GameStats::revenge(eHovercraft attacker, eHovercraft hit)
{
    m_pGameInterface->displayMessage(attacker, hit,
                                   GameInterface::eKillMessage::KILL_MESSAGE_REVENGE);
    stats[hit][IS_DOMINATING_PLAYER_1 + attacker] = false;
}


/*
Denotes a player has picked up a power up.
Increases the powerup count, and adds the score for picking up the powerup.

@param hovercraft   to pickup powerup
*/
void GameStats::pickupPowerup(eHovercraft hovercraft)
{
    addScore(hovercraft, POINTS_GAINED_PICKUP_POWERUP);
    addPowerupCount(hovercraft);
}

/*
Add to the player's total power up count

@param hovercraft   to add powerup count
*/
void GameStats::addPowerupCount(eHovercraft hovercraft)
{
    stats[hovercraft][POWERUPS_TOTAL_PICKED_UP]++;
}

/*
    Fill the vector of endGameStats with EndGameStat values for every
    hovercraft in the game. We can get the hovercraft and beforeAwardsScore now
    but will have to initialize afterAwardsScore and awards later.
*/
void GameStats::initializeEndGameStats()
{
    endGameStats.clear();
    for (pair<int, eHovercraft> kvpair : entityIDToHovercraft)
    {
        EndGameStat stat;
        stat.hovercraft = kvpair.second;
        stat.beforeAwardsScore = get(stat.hovercraft, SCORE_CURRENT);
        stat.afterAwardsScore = stat.beforeAwardsScore;
        stat.awards.clear();
        endGameStats.push_back(stat);
    }
}

vector<EndGameStat> GameStats::getEndGameStats()
{
    initializeEndGameStats();
    awardAwards();
    sortByHighestScoreFirst();
    return endGameStats;
}

struct predicateSortByHighestScore
{
    inline bool operator() (const EndGameStat& left, const EndGameStat& right)
    {
        return (left.afterAwardsScore > right.afterAwardsScore);
    }
};

/*
    Sort winners by the highest score after awards, in decreasing order.
*/
void GameStats::sortByHighestScoreFirst()
{
    std::sort(endGameStats.begin(), endGameStats.end(), predicateSortByHighestScore());
}

/*
    @return all the hovercrafts that have the highest value of the specified stat
            Ignores when the stat is 0.
*/
vector<eHovercraft> GameStats::getHovercraftsThatHaveHighest(eStat stat)
{
    int highest = 0;
    vector<eHovercraft> hovercrafts;

    for (int h = HOVERCRAFT_PLAYER_1; h < MAX_HOVERCRAFT_INDEX; h++)
    {
        eHovercraft hovercraft = static_cast<eHovercraft>(h);
        int value = get(hovercraft, stat);
        if (value > highest)
        {
            highest = value;
            hovercrafts.clear();
        }
        if ((value >= highest) && (highest > 0))
        {
            hovercrafts.push_back(hovercraft);
        }
    }
    return hovercrafts;
}

/*
    Award all hovercrafts with the award of having the highest specified stat.
    Increases afterAwardsStat by point value.

    @param stat         to have the highest value of
    @param name         of the award
    @param description  of the award
    @param points       the award gives
*/
void GameStats::awardHighestStat(eStat stat, string name, string description, int points)
{
    vector<eHovercraft> winners = getHovercraftsThatHaveHighest(stat);
    // for (EndGameStat endStat : endGameStats)
    for (int i = 0, size = endGameStats.size(); i < size; i++)
    {
        if (FuncUtils::contains(winners, endGameStats.at(i).hovercraft))
        {
            endGameStats.at(i).afterAwardsScore += points;
            endGameStats.at(i).awards.push_back(make_tuple(name, description, points));
            cout << "Award " << endGameStats.at(i).hovercraft << " with " << name << ": \"" << description << "\" +" << points << endl;
        }
    }

}

void GameStats::awardAwards()
{
    // Multiplayer only awards
    if (m_iPlayerCount > 1)
    {
        awardHighestStat(KILLS_TOTAL_AGAINST_BOTS,      "Ludite", "Most bot kills",             200);
        awardHighestStat(KILLS_TOTAL_AGAINST_PLAYERS,   "Misanthropist", "Most player kills",   200);
    }
    awardHighestStat(POWERUPS_TOTAL_PICKED_UP,      "Hungry for Power", "Most powerups",    200);
    awardHighestStat(KILLSTREAK_LARGEST,            "Tactical", "Largest killstreak",       100);
}
