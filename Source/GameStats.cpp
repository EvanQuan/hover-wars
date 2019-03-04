#include "GameStats.h"
#include "SoundManager.h"

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
Players gain additional points against other players based on their current
total killstreak. This gives players an incentive to not get hit.
*/
#define POINTS_GAINED_PER_KILLSTREAK 20
/*
Base points for picking up a power up
*/
#define POINTS_GAINED_PICKUP_POWERUP 10
/*
Base points lost for getting hit
*/
#define POINTS_LOST_GOT_HIT 10
/*
Additional points lost per own killstreak. This makes having a large killstreak
risky as you will lose more points.
*/
#define POINTS_LOST_PER_KILLSTREAK 10
/*
Notifies everyone once a player hits a milestone.
*/
#define CURRENT_TOTAL_KILLSTREAK_MILESTONE 5


// Singleton instance
GameStats* GameStats::m_pInstance = nullptr;

GameStats::GameStats()
{
    initialize();
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
*/
void GameStats::initialize()
{
    initializeStats();
    initializeCooldowns();
}

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

/*
Get a stat. For example:

    int killstreak = gameStats.get(PLAYER_1, CURRENT_KILLSTREAKS_AGAINST_PLAYER_2);

will retrieve Player 1's current killstreaks against player 2.
@pure
*/
int GameStats::get(ePlayer player, eStat stat)
{
    return stats[player][stat];
}

/*
Get a cooldown. For example:

     = gameStats.get(PLAYER_1, COOLDOWN_ROCKET);

will retrieve Player 1's current killstreaks against player 2.
@pure
*/
float GameStats::get(ePlayer player, eCooldown cooldown)
{
    return cooldowns[player][cooldown];
}

/*
@return true is ability is ready to be used
@pure
*/
bool GameStats::isOnCooldown(ePlayer player, eCooldown cooldown)
{
    return cooldowns[player][cooldown] <= 0.0f;
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
    case ABILITY_TRAIL_ACTIVATE:
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
    // Update score first
    updateAttackerAndHitScore(playerAttacker, playerHit);

    updateAttackerAndHitKills(playerAttacker, playerHit);
    updateAttackerAndHitKillstreak(playerAttacker, playerHit);

#ifndef NDEBUG
    cout << "Player " << playerAttacker << " hit Player " << playerHit << endl;
    debugPlayer(playerAttacker);
    debugPlayer(playerHit);
#endif
}

void GameStats::debugPlayer(ePlayer player)
{
    cout << "\tPlayer " << player << endl
        << "\t\tscore: " << stats[player][CURRENT_SCORE] << endl
        << "\t\ttotal kills: " << stats[player][TOTAL_KILLS] << endl
        << "\t\tcurrent total killstreak: " << stats[player][CURRENT_TOTAL_KILLSTREAK] << endl
        << "\t\tlargest total killstreak: " << stats[player][LARGEST_TOTAL_KILLSTREAK] << endl
        << "\t\tcurrent killstreak against Player 0: " << getCurrentKillstreakAgainst(player, PLAYER_1) << endl
        << "\t\tcurrent killstreak against Player 1: " << getCurrentKillstreakAgainst(player, PLAYER_2) << endl
        << "\t\tcurrent killstreak against Player 2: " << getCurrentKillstreakAgainst(player, PLAYER_3) << endl
        << "\t\tcurrent killstreak against Player 3: " << getCurrentKillstreakAgainst(player, PLAYER_4) << endl
        << "\t\tis dominating Player 0: " << isDominating(player, PLAYER_1) << endl
        << "\t\tis dominating Player 1: " << isDominating(player, PLAYER_2) << endl
        << "\t\tis dominating Player 2: " << isDominating(player, PLAYER_3) << endl
        << "\t\tis dominating Player 3: " << isDominating(player, PLAYER_4) << endl
        << "\t\ttotal abilities used: " << stats[player][TOTAL_ABILITIES_USED] << endl
        << "\t\ttotal rockets used: " << stats[player][ABILITY_ROCKET_USED] << endl
        << "\t\ttotal spikes used: " << stats[player][ABILITY_SPIKES_USED] << endl
        << "\t\ttotal trails used: " << stats[player][ABILITY_TRAIL_USED] << endl
        << "\t\ttotal dashes used: " << stats[player][ABILITY_DASH_USED] << endl;
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

@pure
*/
int GameStats::getScoreGainedForAttacker(ePlayer playerAttacker, ePlayer playerHit)
{
    int basePoints = POINTS_GAINED_HIT_PLAYER;
    int killstreakBonus = POINTS_GAINED_PER_KILLSTREAK * stats[playerAttacker][CURRENT_TOTAL_KILLSTREAK];
    int revengeBonus = isDominating(playerHit, playerAttacker) ? POINTS_GAINED_HIT_REVENGE : 0;
    return basePoints + killstreakBonus + revengeBonus;
}

/*
Get the score for playerHit to lose if hit by playerAttacker
playerHit cannot lose more points than they have.

@pure
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
Update the attacker's total kills, and total kills against hit player
*/
void GameStats::updateAttackerAndHitKills(ePlayer playerAttacker, ePlayer playerHit)
{
    stats[playerAttacker][TOTAL_KILLS]++;
    if (playerHit == PLAYER_1) {
        stats[playerAttacker][TOTAL_KILLS_AGAINST_PLAYER_1 + playerHit]++;
    }
    else if (playerHit == PLAYER_2) {
        stats[playerAttacker][TOTAL_KILLS_AGAINST_PLAYER_2 + playerHit]++;
    }
    else if (playerHit == PLAYER_3) {
        stats[playerAttacker][TOTAL_KILLS_AGAINST_PLAYER_3 + playerHit]++;
    }
    else if (playerHit == PLAYER_4) {
        stats[playerAttacker][TOTAL_KILLS_AGAINST_PLAYER_4 + playerHit]++;
    }
}

/*
Update the killstreaks from the results of playerAttacker hitting playerHit
*/
void GameStats::updateAttackerAndHitKillstreak(ePlayer playerAttacker, ePlayer playerHit)
{
    // Increase the attacker's killstreak
    addKillstreak(playerAttacker, playerHit);
    // Reset the hit's killstreaks
    resetKillstreak(playerHit, playerAttacker);
}
/*
Add to a playerAttacker's killstreak against playerHit.
playerHit's domination streak against playerAttacker ends.
NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 

This does not moditfy playerHit's data
*/
void GameStats::addKillstreak(ePlayer playerAttacker, ePlayer playerHit)
{
    // Update attacker's current total killstreak
    increaseCurrentTotalKillstreak(playerAttacker);
    // notify if attacker reached current total killstreak milestone
    if (stats[playerAttacker][CURRENT_TOTAL_KILLSTREAK] % CURRENT_TOTAL_KILLSTREAK_MILESTONE == 0)
    {
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_STREAK);
    }

    // Update attacker's current total killstreak against hit
    stats[playerAttacker][CURRENT_KILLSTREAK_AGAINST_PLAYER_1 + playerHit]++;

    // Update dominating if exceeds domination count and not already dominating
    if (canStartDomination(playerAttacker, playerHit))
    {
        dominate(playerAttacker, playerHit);
    }
}


/*
Increase the player's current total killstreak and check for largest total
killstreak updating.
*/
void GameStats::increaseCurrentTotalKillstreak(ePlayer player)
{
    stats[player][CURRENT_TOTAL_KILLSTREAK]++;
    updateLargestTotalKillstreak(player);
}

/*

*/
void GameStats::updateLargestTotalKillstreak(ePlayer player)
{
    if (stats[player][CURRENT_TOTAL_KILLSTREAK] > stats[player][LARGEST_TOTAL_KILLSTREAK])
    {
        stats[player][LARGEST_TOTAL_KILLSTREAK] = stats[player][CURRENT_TOTAL_KILLSTREAK];
    }
}

int GameStats::getCurrentKillstreakAgainst(ePlayer playerAttacker, ePlayer playerHit)
{
    return stats[playerAttacker][CURRENT_KILLSTREAK_AGAINST_PLAYER_1 + playerHit];
}

/*
Reset a playerHit's killstreak against playerAttacker.
NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 
*/
void GameStats::resetKillstreak(ePlayer playerHit, ePlayer playerAttacker)
{
    // Reset current total killstreak
    stats[playerHit][CURRENT_TOTAL_KILLSTREAK] = 0;
    // Reset current total killstreak against attacker
    stats[playerHit][CURRENT_KILLSTREAK_AGAINST_PLAYER_1 + playerAttacker] = 0;

    // If player hit was dominating playerAttacker, disable domination and 
    // player revenge sound.
    if (isDominating(playerHit, playerAttacker))
    {
        revenge(playerAttacker, playerHit);
    }
}

/*
Check whether playerToCheck has a large enough killstreak against playerHit to
count as dominating.

NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 

@return true if playerToCheck is dominating playerHit
*/
bool GameStats::isDominating(ePlayer playerAttacker, ePlayer playerHit)
{
    return stats[playerAttacker][IS_DOMINATING_PLAYER_1 + playerHit];
}

/*
playerAttacker can start domination against playerHIt if
1. playerAttacker's killstreak against playerHit is at least DOMINATION_COUNT
2. playerAttacker is not already dominating playerHit
*/
bool GameStats::canStartDomination(ePlayer playerAttacker, ePlayer playerHit)
{
    return getCurrentKillstreakAgainst(playerAttacker, playerHit) >= DOMINATION_COUNT
        && !isDominating(playerAttacker, playerHit);
}

/*
Enable playerAttacker's domaination status against playerHit
*/
void GameStats::dominate(ePlayer playerAttacker, ePlayer playerHit)
{
    SOUND_MANAGER->play(SoundManager::SOUND_KILL_DOMINATION);
    stats[playerAttacker][IS_DOMINATING_PLAYER_1 + playerHit] = true;
}
/*
Disable playerWasDominating's domination status against playerToGetRevenge.
*/
void GameStats::revenge(ePlayer playerToGetRevenge, ePlayer playerWasDominating)
{
    SOUND_MANAGER->play(SoundManager::SOUND_KILL_REVENGE);
    stats[playerWasDominating][IS_DOMINATING_PLAYER_1 + playerToGetRevenge] = false;
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
