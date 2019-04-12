#include "GameStats.h"
#include "UserInterface/GameInterface.h"
#include "EntityManager.h"
#include "EntityHeaders/HovercraftEntity.h"

/*
Number of killstreaks against another player to count as domination
*/
#define DOMINATION_COUNT 3

/*
    Number of hovercrafts needed in the game to enable speed boost powerup
*/
#define HOVERCRAFT_COUNT_TO_ENABLE_POWERUP 3
/*
After total score gained has been calculated, multiply score gained by value if
hit a bot
*/
#define BOT_SCORE_MULTIPLIER_EASY   1.0
#define BOT_SCORE_MULTIPLIER_MEDIUM 0.5
#define BOT_SCORE_MULTIPLIER_HARD   0.25
/*
Base points gained for hitting a hovercraft
*/
#define POINTS_GAINED_HIT_BASE 50
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

@Deprecated
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
#define CURRENT_TOTAL_KILLSTREAK_MILESTONE 10


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
    Initialize all stats and cooldowns to 0.

    This should be called at the start of every game, or if the game resets.
    It should also be called AFTER players and bots have been initialized.

    @param aiType   if AI on the same team, if they hit each other, there is no
    point change.
*/
void GameStats::reinitialize(int playerCount,
    int botCount,
    eGameMode aiType,
    eBotDifficulty botDifficulty,
    bool scoreLossEnabled)
{
    m_iPlayerCount = playerCount;
    m_iBotCount = botCount;
    m_eGameMode = aiType;
    m_eBotDifficulty = botDifficulty;
    m_bScoreLossEnabled = scoreLossEnabled;
    switch (m_eBotDifficulty)
    {
    case DIFFICULTY_EASY:
        m_eBotScoreMultiplier = BOT_SCORE_MULTIPLIER_EASY;
        break;
    case DIFFICULTY_MEDIUM:
        m_eBotScoreMultiplier = BOT_SCORE_MULTIPLIER_MEDIUM;
        break;
    case DIFFICULTY_HARD:
        m_eBotScoreMultiplier = BOT_SCORE_MULTIPLIER_HARD;
        break;
    }

    initializeStats();
    correspondEntitiesToHovercrafts();
    firstBloodHappened = false;
    queueFirstBlood = false;
    m_eScoreLeaders.clear();

    m_bSpeedBoostEnabled = (playerCount + botCount) >= HOVERCRAFT_COUNT_TO_ENABLE_POWERUP;
}

/*
    Set all stats to 0
*/
void GameStats::initializeStats()
{
    for (int player = HOVERCRAFT_PLAYER_1; player < MAX_HOVERCRAFT_COUNT; player++)
    {
        for (int stat = 0; stat < HOVERCRAFTSTAT_COUNT; stat++)
        {
            stats[player][stat] = 0;
        }
    }

    for (int i = 0; i < GLOBALSTAT_COUNT; i++)
    {
        globalStats[i] = 0;
    }
}

void GameStats::correspondEntitiesToHovercrafts()
{
    entityIDToHovercraft.clear();
    EntityManager* pEntityManager = ENTITY_MANAGER;
    const vector<HovercraftEntity*>* players = pEntityManager->getPlayerList();
    const vector<HovercraftEntity*>* bots = pEntityManager->getBotList();
    HovercraftEntity* player;
    HovercraftEntity* bot;
#ifdef _DEBUG
    cout << "Players:" << endl;
#endif
    int id;
    eHovercraft hovercraft;
    for (size_t i = 0, size = players->size(); i < size; ++i)
    {
        player = players->at(i);
        id = player->getID();
        hovercraft = static_cast<eHovercraft>(i);
        entityIDToHovercraft.insert({id, hovercraft});
        player->correspondToEHovercraft(hovercraft);
#ifdef _DEBUG
        cout << "ID: " << id << " | Hovercraft: " << hovercraft << endl;
#endif // _DEBUG

    }
#ifdef _DEBUG
    cout << "Bots:" << endl;
#endif // _DEBUG

    for (size_t i = 0, size = bots->size(); i < size; ++i)
    {
        bot = bots->at(i);
        id = bot->getID();
        hovercraft = static_cast<eHovercraft>(i + HOVERCRAFT_BOT_1);
        entityIDToHovercraft.insert({id, hovercraft});
        bot->correspondToEHovercraft(hovercraft);
#ifdef _DEBUG
        cout << "ID: " << id << " | Hovercraft: " << hovercraft << endl;
#endif // _DEBUG
    }
}

/*
    Get a stat about a specified hovercraft.

    @param hovercraft   to get stat about
    @param stat         to retrieve
*/
int GameStats::get(eHovercraft hovercraft, eHovercraftStat stat) const
{
    return stats[hovercraft][stat];
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
    checkForNewScoreLeader(hovercraft);
#ifdef _DEBUG
    debugPrintAllScores();
#endif // _DEBUG

}

/*
    Check if the candidate hovercraft is a candidate to be the new score
    leader. If so, update with a new leader notification.
*/
void GameStats::checkForNewScoreLeader(eHovercraft candidate)
{
    int newLargestScore = getLargestScore();
    int oldLargestScore = globalStats[eGlobalStat::SCORE_LARGEST];

    if (newLargestScore != oldLargestScore)
    {
        globalStats[eGlobalStat::SCORE_LARGEST] = newLargestScore;
        updateScoreLeaders(candidate);
    }

}

// Predicate for filtering old leaders
bool notScoreLeader(eHovercraft hovercraft)
{
    return !GAME_STATS->hasLargestScore(hovercraft);
}

/*
    Update the list of score leaders with th addition of the new score leader

    @param candidate    to add to new leaders
*/
void GameStats::updateScoreLeaders(eHovercraft candidate)
{
    // Remove old leaders
    m_eScoreLeaders.erase(std::remove_if(m_eScoreLeaders.begin(),
                                         m_eScoreLeaders.end(),
                                         notScoreLeader),
                          m_eScoreLeaders.end());
    if (!FuncUtils::contains(m_eScoreLeaders, candidate) && hasLargestScore(candidate))
    {
        // Add new leader
        m_eScoreLeaders.push_back(candidate);
        // Display new score leader
        // In order for the first blood notification to have precidence both
        // visually and with audio, we will ignore the leader notifcation until
        // after first blood happened.
        if (firstBloodHappened)
        {
            m_pGameInterface->displayKillMessage(candidate,
                                             HOVERCRAFT_INVALID, /* Hit doesn't matter */
                                             GameInterface::eKillMessage::KILL_MESSAGE_NEW_LEADER);
        } else if (queueFirstBlood) {
            // First blood is queued, so now we can set firstBloodHappened at
            // the end.
            firstBloodHappened = true;
            // To prevent unnecessary future assignments
            queueFirstBlood = false;
        }
    }

}

void GameStats::debugPrintAllScores()
{
    for (int p = 0; p < m_iPlayerCount; p++)
    {
        eHovercraft player = static_cast<eHovercraft>(p);
        cout << "Player " << player << ": " << get(player, SCORE_CURRENT) << endl;
    }
    for (int b = 0; b < m_iBotCount; b++)
    {
        eHovercraft bot = static_cast<eHovercraft>(b + HOVERCRAFT_BOT_1);
        cout << "Bot " << bot <<  ": " << get(bot, SCORE_CURRENT) << endl;
    }

    for (eHovercraft leader : m_eScoreLeaders)
    {
        cout << "Score leader: " << leader  << ": " << get(leader, SCORE_CURRENT) << endl;
    }

    cout << "Highest score: " << get(eGlobalStat::SCORE_LARGEST) << endl;

}

void GameStats::addScore(eHovercraft hovercraft, eAddScoreReason reason, eAbility ability)
{
    addScore(hovercraft, reason);
    if (ability <= ABILITY_TRAIL_ACTIVATE)
    {
        stats[hovercraft][KILLS_WITH_ROCKET + ability]++;
    }
}

void GameStats::reflectRocket(eHovercraft hovercraft)
{
    stats[hovercraft][ROCKETS_REFLECTED]++;
}

/*
    Track that the hovecraft used an ability.
    This is used to track all the past abilities the hovercraft has used for
    that round, which may be needed if post-round achievements are implemented.

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
    Signifies that the attacker hovecraft hit the hit hovercraft.
    Upon getting hit, the attacker gains score, and the hit hovercraft loses
    score.

    Updates killstreaks and scores.
*/
void GameStats::hit(eHovercraft attacker, eHovercraft hit)
{
    if (isOnSameTeam(attacker, hit))
    {
        return;
    }
    // Update score first
    updateAttackerAndHitScore(attacker, hit);

    // Update kill statuses
    updateAttackerAndHitKills(attacker, hit);
    updateAttackerAndHitKillstreak(attacker, hit);

    m_pGameInterface->displayKillMessage(attacker, hit, GameInterface::KILL_MESSAGE_KILL);

    debug(attacker);
    debug(hit);
#ifndef NDEBUG
    // cout << "Player " << attacker << " hit Player " << attacker << endl;
    // debug(attacker);
    // debug(hit);
#endif
}

void GameStats::debug(eHovercraft hovercraft)
{
    cout << "\t " << (hovercraft <= HOVERCRAFT_PLAYER_4 ? "Player " + to_string((int)hovercraft) : "Bot " + to_string((int)(hovercraft - HOVERCRAFT_BOT_1))) << endl
        << "\t\tscore: " << stats[hovercraft][SCORE_CURRENT] << endl
        << "\t\ttotal kills: " << stats[hovercraft][KILLS_TOTAL] << endl
        << "\t\tcurrent total killstreak: " << stats[hovercraft][KILLSTREAK_CURRENT] << endl
        << "\t\tlargest total killstreak: " << stats[hovercraft][KILLSTREAK_LARGEST] << endl
        << "\t\tcurrent killstreak against Player 0: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_1) << endl
        << "\t\tcurrent killstreak against Player 1: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_2) << endl
        << "\t\tcurrent killstreak against Player 2: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_3) << endl
        << "\t\tcurrent killstreak against Player 3: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_PLAYER_4) << endl
        << "\t\tcurrent killstreak against Bot 0: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_BOT_1) << endl
        << "\t\tcurrent killstreak against Bot 1: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_BOT_2) << endl
        << "\t\tcurrent killstreak against Bot 2: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_BOT_3) << endl
        << "\t\tcurrent killstreak against Bot 3: " << getCurrentKillstreakAgainst(hovercraft, HOVERCRAFT_BOT_4) << endl
        << "\t\tis dominating Player 0: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_1) << endl
        << "\t\tis dominating Player 1: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_2) << endl
        << "\t\tis dominating Player 2: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_3) << endl
        << "\t\tis dominating Player 3: " << isDominating(hovercraft, HOVERCRAFT_PLAYER_4) << endl
        << "\t\tis dominating Bot 0: " << isDominating(hovercraft, HOVERCRAFT_BOT_1) << endl
        << "\t\tis dominating Bot 1: " << isDominating(hovercraft, HOVERCRAFT_BOT_2) << endl
        << "\t\tis dominating Bot 2: " << isDominating(hovercraft, HOVERCRAFT_BOT_3) << endl
        << "\t\tis dominating Bot 3: " << isDominating(hovercraft, HOVERCRAFT_BOT_4) << endl
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
    Get the score for attacker to gain if they hit hit
    Checks for first blood.

    @param attacker     to gain score
    @param hit          that attacker hit
*/
int GameStats::getScoreGainedForAttacker(eHovercraft attacker, eHovercraft hit)
{
    int basePoints = POINTS_GAINED_HIT_BASE;
    int killstreakBonus = POINTS_GAINED_PER_KILLSTREAK * FuncUtils::max(stats[attacker][KILLSTREAK_CURRENT], 0);
    int killstreakEndingBonus = POINTS_GAINED_PER_HIT_KILLSTREAK * FuncUtils::max(stats[hit][KILLSTREAK_CURRENT], 0);
    int revengeBonus = isDominating(hit, attacker) ? POINTS_GAINED_HIT_REVENGE : 0;
    int firstBloodBonus;
    if (firstBloodHappened) {
        firstBloodBonus = 0;
    } else {
        firstBloodBonus = POINTS_GAINED_FIRST_BLOOD;
        queueFirstBlood = true;
        m_pGameInterface->displayKillMessage(attacker, hit, GameInterface::eKillMessage::KILL_MESSAGE_FIRST_BLOOD);
    }
    int totalGained = basePoints + killstreakBonus + killstreakEndingBonus + revengeBonus + firstBloodBonus;
    if (isBot(hit))
    {
        totalGained = static_cast<int>(totalGained * m_eBotScoreMultiplier);
    }
    return totalGained;
}

/*
    Get the score for hit to lose if hit by attacker
    hit cannot lose more points than they have.

    @param attacker     that caused hit to lose score  
                        @EvanQuan: Not Referenced Parameter
                        @NOTE Right now, the attacker has no impact on the
                        score lost for hit. I will keep this as a parameter for
                        now, but will
                        remove it if decided it is not needed.
    @param hit          to lose score

*/
int GameStats::getScoreLostForHit(eHovercraft attacker, eHovercraft hit) const
{
    if (!m_bScoreLossEnabled)
    {
        return 0;
    }
    int basePoints = POINTS_LOST_GOT_HIT;
    int killstreakBonus = POINTS_LOST_PER_KILLSTREAK * FuncUtils::max(stats[hit][KILLSTREAK_CURRENT], 0);
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

    // Update team scores if team game mode
    switch (m_eGameMode)
    {
    case GAMEMODE_TEAMS_AI_VS_PLAYERS:
        if (isBot(attacker))
        {
            globalStats[TEAM_BOT_SCORE] += points;
        }
        else
        {
            globalStats[TEAM_PLAYER_SCORE] += points;
        }
        break;
    case GAMEMODE_TEAM_AI_SOLO_PLAYERS:
        if (isBot(attacker))
        {
            // Only bots have a team
            globalStats[TEAM_BOT_SCORE] += points;
        }
        break;
    }
}

void GameStats::removeScore(eHovercraft hit, int points)
{
    stats[hit][SCORE_CHANGE] = -points;
    stats[hit][SCORE_CURRENT] -= points;

    // Update team scores if team game mode
    switch (m_eGameMode)
    {
    case GAMEMODE_TEAMS_AI_VS_PLAYERS:
        if (isBot(hit))
        {
            globalStats[TEAM_BOT_SCORE] = FuncUtils::max(globalStats[TEAM_BOT_SCORE] - points, 0);
        }
        else
        {
            globalStats[TEAM_PLAYER_SCORE] = FuncUtils::max(globalStats[TEAM_PLAYER_SCORE] - points, 0);
        }
        break;
    case GAMEMODE_TEAM_AI_SOLO_PLAYERS:
        if (isBot(hit))
        {
            // Only bots have a team
            globalStats[TEAM_BOT_SCORE] = FuncUtils::max(globalStats[TEAM_BOT_SCORE] - points, 0);
        }
        break;
    }
}

bool GameStats::hasLargestScore(eHovercraft hovercraft)
{
    return get(eGlobalStat::SCORE_LARGEST) == get(hovercraft, SCORE_CURRENT);
}

int GameStats::getLargestScore()
{
    int largestScore = 0;
    for (int hovercraft = 0; hovercraft < MAX_HOVERCRAFT_COUNT; hovercraft++)
    {
        largestScore = FuncUtils::max(stats[hovercraft][SCORE_CURRENT], largestScore);
    }
    return largestScore;
}

/*
    Update the attacker's total kills, and total kills against hit player
*/
void GameStats::updateAttackerAndHitKills(eHovercraft attacker, eHovercraft hit)
{
    stats[attacker][KILLS_TOTAL]++;
    stats[hit][DEATHS_TOTAL]++;
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

bool GameStats::isBot(HovercraftEntity* hovercraft) const
{
    return (hovercraft != nullptr) && isBot(hovercraft->getID());
}

bool GameStats::isPlayer(HovercraftEntity* hovercraft) const
{
    return (hovercraft != nullptr) && isPlayer(hovercraft->getID());
}


bool GameStats::isBot(eHovercraft hovercraft) const
{
    return HOVERCRAFT_BOT_1 <= hovercraft && hovercraft <= HOVERCRAFT_BOT_4;
}

bool GameStats::isPlayer(eHovercraft hovercraft) const
{
    return HOVERCRAFT_PLAYER_1 <= hovercraft && hovercraft <= HOVERCRAFT_PLAYER_4;
}

bool GameStats::isOnSameTeam(eHovercraft hovercraft1, eHovercraft hovercraft2) const
{
    switch (m_eGameMode)
    {
    case GAMEMODE_FREE_FOR_ALL:
        return false;
        break;
    case GAMEMODE_TEAMS_AI_VS_PLAYERS:
        return (isPlayer(hovercraft1) && isPlayer(hovercraft2))
            || (isBot(hovercraft1) && isBot(hovercraft2));
        break;
    case GAMEMODE_TEAM_AI_SOLO_PLAYERS:
        return isBot(hovercraft1) && isBot(hovercraft2);
        break;
    default:
        return false;
    }
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
        // Only display kill message if players are involved (either attacker or hit).
        // This avoids unnecessary work to do and sound effects.
        if (isPlayer(attacker) || isPlayer(hit))
        {
            m_pGameInterface->displayKillMessage(attacker, hit,
                GameInterface::eKillMessage::KILL_MESSAGE_KILLSTREAK);
        }
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
    stats[hovercraft][KILLSTREAK_CURRENT] = FuncUtils::max(1, stats[hovercraft][KILLSTREAK_CURRENT] + 1);
    cout << hovercraft << " killstreak increase to " << stats[hovercraft][KILLSTREAK_CURRENT] << endl;
    updateLargestTotalKillstreak(hovercraft);
}

/*
    If the specified hovercraft's current killstreak is largre than their
    largest killstreak, then update their largest killstreak to match it.
*/
void GameStats::updateLargestTotalKillstreak(eHovercraft hovercraft)
{
    stats[hovercraft][KILLSTREAK_LARGEST] = FuncUtils::max(stats[hovercraft][KILLSTREAK_LARGEST],
                                                           stats[hovercraft][KILLSTREAK_CURRENT]);
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
    cout << hit << " killstreak before "  << stats[hit][KILLSTREAK_CURRENT] << endl;
    // Reset current total killstreak
    if (stats[hit][KILLSTREAK_CURRENT] > 0)
    {
        stats[hit][KILLSTREAK_CURRENT] = 0;
        cout << hit << " killstreak " << " reset" << endl;
    }

    // Decrease killstreak to negative
    stats[hit][KILLSTREAK_CURRENT]--;
    cout << hit << " killstreak is now "  << stats[hit][KILLSTREAK_CURRENT] << endl;

    // Reset current total killstreak against attacker
    if (stats[hit][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + attacker] > 0)
    {
        stats[hit][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + attacker] = 0;
    }
    stats[hit][KILLSTREAK_CURRENT_AGAINST_PLAYER_1 + attacker]--;

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
    if (!(isBot(attacker) && isBot(hit)))
    {
        m_pGameInterface->displayKillMessage(attacker, hit,
            GameInterface::eKillMessage::KILL_MESSAGE_DOMINATION);

    }
    stats[attacker][IS_DOMINATING_PLAYER_1 + hit] = true;
}
/*
    Disable hit's domination status against attacker.

    @param attacker     to get revenge on hit
    @param hit          was dominating attacker
*/
void GameStats::revenge(eHovercraft attacker, eHovercraft hit)
{
    if (!(isBot(attacker) && isBot(hit)))
    {
        m_pGameInterface->displayKillMessage(attacker, hit,
            GameInterface::eKillMessage::KILL_MESSAGE_REVENGE);
    }
    stats[hit][IS_DOMINATING_PLAYER_1 + attacker] = false;
}


/*
    Denotes a player has picked up a power up.
    Increases the powerup count, and adds the score for picking up the powerup.

    @param hovercraft   to pickup powerup
*/
void GameStats::pickupPowerup(eHovercraft hovercraft)
{
    // addScore(hovercraft, POINTS_GAINED_PICKUP_POWERUP);
    addPowerupCount(hovercraft);
    // hard coded for now
    m_pGameInterface->displayPowerup(hovercraft, POWERUP_SPEED_BOOST);
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

/*
    Get a vector of EndGameStats for all participating hovercrafts
    of the most recent game round.
*/
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
    @param stat to have the highest non-zreo value of.
    @return all the hovercrafts that have the highest value of the specified stat
            Ignores when the stat is 0.
*/
vector<eHovercraft> GameStats::getHovercraftsThatHaveHighestNonZero(eHovercraftStat stat)
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
    Get all the hovercrafts that have the lowest specified stat, including if
    the stat value is zero.
    @param stat     to have the lowest value of.
*/
vector<eHovercraft> GameStats::getHovercraftsThatHaveLowest(eHovercraftStat stat)
{
    int lowest = numeric_limits<int>::max();
    int value;
    vector<eHovercraft> hovercrafts;

    for (int p = HOVERCRAFT_PLAYER_1; p < m_iPlayerCount; p++)
    {
        eHovercraft player = static_cast<eHovercraft>(p);
        value = get(player, stat);
        if (value < lowest)
        {
            lowest = value;
            hovercrafts.clear();
        }
        if (value <= lowest)
        {
            hovercrafts.push_back(player);
        }
    }
    for (int b = HOVERCRAFT_BOT_1; b < HOVERCRAFT_BOT_1 + m_iBotCount; b++)
    {
        eHovercraft bot = static_cast<eHovercraft>(b);
        value = get(bot, stat);
        if (value < lowest)
        {
            lowest = value;
            hovercrafts.clear();
        }
        if (value <= lowest)
        {
            hovercrafts.push_back(bot);
        }
    }
    return hovercrafts;
}

/*
    @param  stat    to have 0 of
    @return all the hovercrafts that have the specified stat value of 0. Only
            includes hovercrafts that were included in this gamee.

*/
vector<eHovercraft> GameStats::getHovercraftsThatHaveZero(eHovercraftStat stat)
{
    vector<eHovercraft> hovercrafts;

    // Players
    for (int h = HOVERCRAFT_PLAYER_1; h < m_iPlayerCount; h++)
    {
        eHovercraft player = static_cast<eHovercraft>(h);
        if (get(player, stat) == 0)
        {
            hovercrafts.push_back(player);
        }
    }
    // Bots
    for (int b = HOVERCRAFT_BOT_1; b < HOVERCRAFT_BOT_1 + m_iBotCount; b++)
    {
        eHovercraft bot = static_cast<eHovercraft>(b);
        if (get(bot, stat) == 0)
        {
            hovercrafts.push_back(bot);
        }
    }
    return hovercrafts;
}


/*
    Award a specified hovercraft with a specified award.
*/
void GameStats::awardToHovercrafts(eHovercraftStat stat,
                                   string name,
                                   string description,
                                   int points,
                                   vector<eHovercraft> winners)
{
    for (int i = 0, size = endGameStats.size(); i < size; i++)
    {
        eHovercraft hovercraft = endGameStats.at(i).hovercraft;
        if (FuncUtils::contains(winners, hovercraft))
        {
            endGameStats.at(i).afterAwardsScore += points;
            Award award;
            award.name = name;
            award.description = description;
            award.points = points;
            award.statValue = stats[hovercraft][stat];
            endGameStats.at(i).awards.push_back(award);
            //cout << "Award "
            //    << hovercraft
            //    << " with " << name << ": \"" <<
            //    description << " of " << award.statValue
            //   << "\" +" << points << endl;
        }
    }
}
/*
    Award all hovercrafts with the award of having the highest specified stat.
    That specified stat must have a value strictly greater than 0 to count,
    otherwise it it not awarded.
    Increases afterAwardsStat by point value.

    @param stat         to have the highest value of
    @param name         of the award
    @param description  of the award
    @param points       the award gives
*/
void GameStats::awardHighestNonZeroStat(eHovercraftStat stat,
                                        string name,
                                        string description,
                                        int points)
{
    vector<eHovercraft> winners = getHovercraftsThatHaveHighestNonZero(stat);
    awardToHovercrafts(stat, name, description, points, winners);

}

void GameStats::awardLowestStat(eHovercraftStat stat,
                                       string name,
                                       string description,
                                       int points)
{
    vector<eHovercraft> winners = getHovercraftsThatHaveLowest(stat);
    awardToHovercrafts(stat, name, description, points, winners);

}

/*
    Award all hovercrafts with the award of having the specified stat with a
    value of exactly 0.

    @param stat         to have the value of 0
    @param name         of the award
    @param description  of the award
    @param points       the award gives
*/
void GameStats::awardZeroStat(eHovercraftStat stat,
                              string name,
                              string description,
                              int points)
{
    vector<eHovercraft> winners = getHovercraftsThatHaveZero(stat);
    awardToHovercrafts(stat, name, description, points, winners);
}

/*
    Award all awards to the respective winners. Will only award to players and
    bots actually present in the current game.
*/
void GameStats::awardAwards()
{
    // Multiplayer only awards
    if ((m_iPlayerCount > 1) && (m_iBotCount > 1))
    {
        awardHighestNonZeroStat(KILLS_TOTAL_AGAINST_BOTS,      "Ludite",        "Most bot kills",           200);
        awardHighestNonZeroStat(KILLS_TOTAL_AGAINST_PLAYERS,   "Misanthropist", "Most player kills",        200);
    }
    awardHighestNonZeroStat(POWERUPS_TOTAL_PICKED_UP,      "Gotta Go Fast!",    "Most speed boosts",        200);
    awardHighestNonZeroStat(KILLSTREAK_LARGEST,            "Serial Killer",     "Largest killstreak",       100);
    awardHighestNonZeroStat(KILLS_WITH_ROCKET,             "Rocket Man",        "Most rocket kills",        100);
    awardHighestNonZeroStat(KILLS_WITH_TRAIL,              "Pyromaniac",        "Most flame trail kills",   200);
    awardHighestNonZeroStat(KILLS_WITH_SPIKES,             "Porcupine",         "Most spike kills",         300);
    awardHighestNonZeroStat(DEATHS_TOTAL,                  "Consolation",       "Most deaths",              100);
    awardHighestNonZeroStat(ROCKETS_REFLECTED,             "Mirror, Mirror",    "Most rocket reflects",     300);
    awardLowestStat(DEATHS_TOTAL,                          "Survivor",          "Least deaths",             500);
    // Special awards
    awardZeroStat(DEATHS_TOTAL,                            "Untouchable",       "Zero deaths",              500);
}
