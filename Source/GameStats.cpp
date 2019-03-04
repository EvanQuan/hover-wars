#include "GameStats.h"
#include "SoundManager.h"
#include "UserInterface/UserInterface.h"

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
#define POINTS_LOST_GOT_HIT 30
/*
Additional points lost per own killstreak. This makes having a large killstreak
risky as you will lose more points.
*/
#define POINTS_LOST_PER_KILLSTREAK 10
/*
Notifies a killstreak message once player hits a milestone.
*/
#define CURRENT_TOTAL_KILLSTREAK_MILESTONE DOMINATION_COUNT


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
    return stats[FuncUtils::playerToHovercraft(player)][stat];
}
int GameStats::get(eBot bot, eStat stat)
{
    return stats[FuncUtils::botToHovercraft(bot)][stat];
}
int GameStats::get(eHovercraft hovercraft, eStat stat)
{
    return stats[hovercraft][stat];
}

/*
Get a cooldown. For example:

     = gameStats.get(PLAYER_1, COOLDOWN_ROCKET);

will retrieve Player 1's current killstreaks against player 2.
@pure
*/
float GameStats::get(ePlayer player, eCooldown cooldown)
{
    return cooldowns[FuncUtils::playerToHovercraft(player)][cooldown];
}
float GameStats::get(eBot bot, eCooldown cooldown)
{
    return cooldowns[FuncUtils::botToHovercraft(bot)][cooldown];
}
float GameStats::get(eHovercraft hovercraft, eCooldown cooldown)
{
    return cooldowns[hovercraft][cooldown];
}

/*
@return true is ability is ready to be used
@pure
*/
bool GameStats::isOnCooldown(ePlayer player, eCooldown cooldown)
{
    return isOnCooldown(FuncUtils::playerToHovercraft(player), cooldown);
}
bool GameStats::isOnCooldown(eBot bot, eCooldown cooldown)
{
    return isOnCooldown(FuncUtils::botToHovercraft(bot), cooldown);
}
bool GameStats::isOnCooldown(eHovercraft hovercraft, eCooldown cooldown)
{
    return cooldowns[hovercraft][cooldown] <= 0.0f;
}

/*
Add score to the specified player for the specified reason.
*/
void GameStats::addScore(ePlayer player, eAddScoreReason reason)
{
    addScore(FuncUtils::playerToHovercraft(player), reason);
}
void GameStats::addScore(eBot bot, eAddScoreReason reason)
{
    addScore(FuncUtils::botToHovercraft(bot), reason);
}
void GameStats::addScore(eHovercraft hovercraft, eAddScoreReason reason)
{
    switch (reason)
    {
    case HIT_BOT_1:
    case HIT_BOT_2:
    case HIT_BOT_3:
    case HIT_BOT_4:
        // hitBot(hovecraft, scoreReasonToBot.at(reason));
        // break;
    case HIT_PLAYER_1:
    case HIT_PLAYER_2:
    case HIT_PLAYER_3:
    case HIT_PLAYER_4:
        // hitPlayer(hovecraft, scoreReasonToPlayer.at(reason));
        hit(hovercraft, scoreReasonToHovercraft.at(reason));
        break;
    case PICKUP_POWERUP:
        pickupPowerup(hovercraft);
        break;
    }
}

/*
Track that the player used an ability.
*/
void GameStats::useAbility(ePlayer player, eAbility ability)
{
    useAbility(FuncUtils::playerToHovercraft(player), ability);
}
void GameStats::useAbility(eBot bot, eAbility ability)
{
    useAbility(FuncUtils::botToHovercraft(bot), ability);
}
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
Signifies that attacker hit a bot.
*/
// void GameStats::hitBot(ePlayer attacker, eBot botHit)
// {
    // addScore(attacker, POINTS_GAINED_HIT_BOT);
// }
/*
Signifies that attacker hit hit

Updates killstreaks and scores.
*/
void GameStats::hit(eHovercraft attacker, eHovercraft hit)
{
    // Update score first
    updateAttackerAndHitScore(attacker, hit);

    updateAttackerAndHitKills(attacker, hit);
    updateAttackerAndHitKillstreak(attacker, hit);

#ifndef NDEBUG
    cout << "Player " << attacker << " hit Player " << attacker << endl;
    debug(attacker);
    debug(hit);
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
*/
void GameStats::updateAttackerAndHitScore(eHovercraft attacker, eHovercraft hit)
{
    addScore(attacker, getScoreGainedForAttacker(attacker, hit));
    removeScore(hit, getScoreLostForHit(attacker, hit));
}

/*
Get the score for playerAtacker to gain if they hit hit

@pure
*/
int GameStats::getScoreGainedForAttacker(eHovercraft attacker, eHovercraft hit)
{
    int basePoints = FuncUtils::hovercraftToPlayer(attacker) != PLAYER_INVALID ? POINTS_GAINED_HIT_PLAYER : POINTS_GAINED_HIT_BOT;
    int killstreakBonus = POINTS_GAINED_PER_KILLSTREAK * stats[attacker][KILLSTREAK_CURRENT];
    int revengeBonus = isDominating(hit, attacker) ? POINTS_GAINED_HIT_REVENGE : 0;
    return basePoints + killstreakBonus + revengeBonus;
}

/*
Get the score for hit to lose if hit by attacker
hit cannot lose more points than they have.

@pure
*/
int GameStats::getScoreLostForHit(eHovercraft attacker, eHovercraft hit)
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
NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 

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
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_STREAK);
        USER_INTERFACE->displayMessage(FuncUtils::hovercraftToPlayer(attacker), "You have a killstreak of " + std::to_string(killstreak));
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

int GameStats::getCurrentKillstreakAgainst(eHovercraft attacker, eHovercraft playerHit)
{
    return stats[attacker][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + playerHit];
}

/*
Reset a hit's killstreak against attacker.
NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 
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
Check whether playerToCheck has a large enough killstreak against hit to
count as dominating.

NOTE: Only use PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4 

@return true if playerToCheck is dominating hit
*/
bool GameStats::isDominating(eHovercraft playerAttacker, eHovercraft playerHit)
{
    return stats[playerAttacker][IS_DOMINATING_PLAYER_1 + playerHit];
}

/*
attacker can start domination against playerHIt if
1. attacker's killstreak against hit is at least DOMINATION_COUNT
2. attacker is not already dominating hit
*/
bool GameStats::canStartDomination(eHovercraft playerAttacker, eHovercraft playerHit)
{
    return getCurrentKillstreakAgainst(playerAttacker, playerHit) >= DOMINATION_COUNT
        && !isDominating(playerAttacker, playerHit);
}

/*
Enable attacker's domaination status against hit
*/
void GameStats::dominate(eHovercraft playerAttacker, eHovercraft playerHit)
{
    ePlayer player = FuncUtils::hovercraftToPlayer(playerAttacker);
    SOUND_MANAGER->play(SoundManager::SOUND_KILL_DOMINATION);
    // Ad hoc for single player
    USER_INTERFACE->displayMessage(FuncUtils::hovercraftToPlayer(playerAttacker), "You now are dominating Player " + std::to_string(playerHit + 1));
    stats[playerAttacker][IS_DOMINATING_PLAYER_1 + playerHit] = true;
}
/*
Disable playerWasDominating's domination status against playerToGetRevenge.
*/
void GameStats::revenge(eHovercraft playerToGetRevenge, eHovercraft playerWasDominating)
{
    SOUND_MANAGER->play(SoundManager::SOUND_KILL_REVENGE);
    // Ad hoc for single player
    USER_INTERFACE->displayMessage(FuncUtils::hovercraftToPlayer(playerToGetRevenge), "You got revenge from Player " + std::to_string(playerWasDominating + 1));
    stats[playerWasDominating][IS_DOMINATING_PLAYER_1 + playerToGetRevenge] = false;
}


/*
Denotes a player has picked up a power up
*/
void GameStats::pickupPowerup(eHovercraft hovercraft)
{
    addScore(hovercraft, POINTS_GAINED_PICKUP_POWERUP);
    addPowerupCount(hovercraft);
}

/*
Add to the player's total power up count
*/
void GameStats::addPowerupCount(eHovercraft hovercraft)
{
    stats[hovercraft][POWERUPS_TOTAL_PICKED_UP]++;
}
