#include "UserInterface/GameInterface.h"
#include "EntityManager.h"
#include "SoundManager.h"
#include "EntityHeaders/HovercraftEntity.h"
#include "GameManager.h"


/***********\
 * Defines *
\***********/

// UI Component locations
// These should all be relative to the window dimensions
// not hardcoded pixel values, as they are now.
#define COLOR_WHITE             vec3(1.0)
#define COLOR_RED               vec3(1.0, 0.0, 0.0)
#define COLOR_GREEN             vec3(0.0, 1.0, 0.0)
#define COLOR_YELLOW            vec3(1.0, 1.0, 0.0)

#define COLOR_READY             COLOR_GREEN
#define COLOR_MID_READY         COLOR_YELLOW
#define COLOR_NOT_READY         COLOR_RED

#define COOLDOWN_READY          "Ready"
#define COOLDOWN_DECIMAL_PLACES 1
#define SCORE_SCALE             1.0f
#define SCORE_COLOR             COLOR_WHITE

/*
All user interface components have a location relative to the window dimensions
to ensure that the UI scales with window resizes.

Coordinate system:

(0, 1)                  (1, 1) 

(x=0, y=0)              (1, 0)
*/
#define TIME_SCALE              1.0f
#define TIME_COLOR              COLOR_WHITE
#define TIME_WARNING_MINOR_COLOR     COLOR_YELLOW
#define TIME_WARNING_MAJOR_COLOR     COLOR_RED
// Seconds until game time changes color
#define TIME_WARNING_MINOR      30
#define TIME_WARNING_MAJOR      10
// Time to start the major time warning countdown tick.
#define TIME_WARNING_MAJOR_START TIME_WARNING_MAJOR
#define TIME_WARNING_MINOR_START TIME_WARNING_MINOR

#define TRAIL_SCALE             1.0f

#define ROCKET_SCALE            1.0f

#define SPIKES_SCALE            1.0f

#define DASH_SCALE              1.0f

#define SCORE_CHANGE_DURATION   1.0f
#define SCORE_CHANGE_SCALE      1.0f
#define SCORE_CHANGE_ADD_COLOR  COLOR_GREEN
#define SCORE_CHANGE_SUB_COLOR  COLOR_RED

#define MESSAGE_DURATION        3.0f
#define MESSAGE_SCALE           1.0f
#define MESSAGE_COLOR           COLOR_WHITE

#define NOTIFICATION_DURATION   MESSAGE_DURATION
#define RESUME_TICK_DURATION 1.0f
#define RESUME_COUNTDOWN_DURATION COUNTDOWN_TICKS * RESUME_TICK_DURATION

#define COUNTDOWN_3_COLOR COLOR_RED
#define COUNTDOWN_2_COLOR COLOR_YELLOW
#define COUNTDOWN_1_COLOR COLOR_GREEN
#define COUNTDOWN_GO_COLOR COLOR_WHITE

// Singleton instance
GameInterface* GameInterface::m_pInstance = nullptr;

GameInterface::GameInterface() : UserInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Time
        {0.47f, 0.9f},
        // 1 Trail
        {0.2f, 0.3f},
        // 2 Spikes
        {0.2f, 0.2f},
        // 3 Rocket
        {0.7f, 0.3f},
        // 4 Dash
        {0.7f, 0.2f},
        // 5 Score
        {0.2f, 0.9f},
        // 6 Score Change
        {0.47f, 0.58f},
        // 7 Message
        {0.37f, 0.65f},
        // 8 Powerup
        {0.45f, 0.33f}, 
        // 9 Notification
        {0.42f, 0.75f},
        // 10 Resume game countdown
        {0.48f, 0.50f},
        // 11 Kill/ deaths
        {0.7f, 0.9f},
        // 12 Team bot score
        {0.2f, 0.9f},
        // 13 Team player score
        {0.2f, 0.9f},
        // 14 Team player score 2
        {0.2f, 0.9f},
        // 15 Killstreak
        {0.7f, 0.9f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Time
        {0.0f, 0.0f},
        // 1 Trail
        {0.0f, 0.0f},
        // 2 Spikes
        {0.0f, 0.0f},
        // 3 Rocket
        {0.0f, 0.0f},
        // 4 Dash
        {0.0f, 0.0f},
        // 5 Score
        {0.0f, 0.0f},
        // 6 Score Change
        {0.0f, 0.0f},
        // 7 Message
        {0.0f, 0.0f},
        // 8 Powerup
        {0.0f, 0.0f},
        // 9 Notification
        {0.0f, 0.0f},
        // 10 Resume game countdown
        {0.0f, 0.0f},
        // 11 Kill/deaths
        {0.0f, 0.0f},
        // 12 Team bot score
        {0.0f, -50.0f},
        // 13 Team player score
        {0.0f, -100.0f},
        // 14 Team player score 2
        {0.0f, -150.0f},
        // 15 Killstreak
        {0.0f, -50.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
    setDisplayCount(1);
    m_eHovercraftFocus = HOVERCRAFT_PLAYER_1;
    m_pEntityMngr = ENTITY_MANAGER;
    m_pSoundManager = SOUND_MANAGER;
}

GameInterface* GameInterface::getInstance(int iWidth, int iHeight)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new GameInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}

GameInterface::~GameInterface()
{
}



/*
Display a message for a givne hovercraft's UI for a short duration.
Depending on the message type, a sound may be played, and an alternate message
may be played for other displayed UI's.

@param attacker     of kill
@param hit          of kill
@param message      to display
*/
void GameInterface::displayKillMessage(eHovercraft attacker, eHovercraft hit, eKillMessage message)
{
    bool attackerIsPlayer = GAME_STATS->isPlayer(attacker);
    bool hitIsPlayer = GAME_STATS->isPlayer(hit);
    bool playerInvolved = attackerIsPlayer || hitIsPlayer;
    switch (message)
    {
    case KILL_MESSAGE_DOMINATION:
        m_pSoundManager->play(SoundManager::SOUND_KILL_DOMINATION, playerInvolved);
        displayMessage(attacker,
                       "You are now dominating " + m_eHovercraftToString.at(hit),
                       GAME_MANAGER->getHovercraftColor(hit));
        displayMessage(hit,
                       m_eHovercraftToString.at(attacker) + " is now dominating you",
                       GAME_MANAGER->getHovercraftColor(attacker));
        break;
    case KILL_MESSAGE_FIRST_BLOOD:
        m_pSoundManager->play(SoundManager::eSoundEvent::SOUND_KILL_FIRST_BLOOD);
        // Announce first blood to everyone
        for (int player = 0, playerCount = GAME_STATS->getPlayerCount();
             player < playerCount;
             player++)
        {
            string attackerName;
            string hitName;
            vec3 color;
            if (attacker == player)
            {
                attackerName = "You";
                hitName = m_eHovercraftToString.at(hit);
                color = GAME_MANAGER->getHovercraftColor(hit);
            }
            else if (hit == player)
            {
                attackerName = m_eHovercraftToString.at(attacker);
                hitName = "you";
                color = GAME_MANAGER->getHovercraftColor(attacker);
            }
            else 
            {
                attackerName = m_eHovercraftToString.at(attacker);
                hitName = m_eHovercraftToString.at(hit);
                color = GAME_MANAGER->getHovercraftColor(attacker);
            }
            displayMessage(
                static_cast<eHovercraft>(player),
                attackerName + " got first blood against " + hitName,
                color);
        }
        // displayKillMessage(attacker, "You got first blood against " + m_eHovercraftToString.at(hit));
        // displayKillMessage(hit, m_eHovercraftToString.at(attacker) + " got first blood against you");
        break;
    case KILL_MESSAGE_REVENGE:
        m_pSoundManager->play(SoundManager::SOUND_KILL_REVENGE, playerInvolved);
        displayMessage(attacker,
                       "You got revenge from " + m_eHovercraftToString.at(hit),
                        GAME_MANAGER->getHovercraftColor(hit));
        displayMessage(hit,
                       m_eHovercraftToString.at(attacker) + " got revenge from you",
                       GAME_MANAGER->getHovercraftColor(attacker));
        break;
    case KILL_MESSAGE_KILLSTREAK:
        m_pSoundManager->play(SoundManager::SOUND_KILL_STREAK, playerInvolved);
        displayMessage(attacker, "You have a killstreak of "
            + std::to_string(GAME_STATS->get(attacker, GameStats::eHovercraftStat::KILLSTREAK_CURRENT)),
            MESSAGE_COLOR);
        break;
    case KILL_MESSAGE_KILL:
        m_fScoreChangeTimes[attacker] = SCORE_CHANGE_DURATION;
        m_fScoreChangeTimes[hit] = SCORE_CHANGE_DURATION;
        break;
    case KILL_MESSAGE_NEW_LEADER:
        m_pSoundManager->play(SoundManager::SOUND_UI_NEW_LEADER);
        for (int player = 0, playerCount = MAX_HOVERCRAFT_COUNT;
            player < playerCount;
            player++)
        {
            string newLeaderName;
            vec3 color;
            eHovercraft hovercraft = static_cast<eHovercraft>(player);
            if (attacker == hovercraft)
            {
                newLeaderName = "You are";
                color = MESSAGE_COLOR;
            }
            else
            {
                newLeaderName = m_eHovercraftToString.at(attacker) + " is";
                color = GAME_MANAGER->getHovercraftColor(attacker);
            }
            displayMessage(hovercraft,
                newLeaderName + " now in the lead",
                color);
        }
        break;
    }
}

void GameInterface::displayNotification(eNotification message)
{
    switch (message)
    {
    case NOTIFICATION_TIME_MINOR:
        m_sNotification = std::to_string(TIME_WARNING_MINOR) + "s remaining";
        break;
    case NOTIFICATION_TIME_MAJOR:
        m_sNotification = std::to_string(TIME_WARNING_MAJOR) + "s remaining";
        break;
    }
    m_fNotificationTime = NOTIFICATION_DURATION;
}

void GameInterface::displayPowerup(eHovercraft hovercraft, ePowerup powerup)
{
    string message = "";
    switch (powerup)
    {
    case POWERUP_SPEED_BOOST:
        message = "Speed Boost";
        break;
    }
    m_sPowerupMessages[hovercraft] = message;
    m_fPowerupMessageTimes[hovercraft] = POWERUP_TIME;
}

/*
Display a message for a given hovercraft's UI for a short duration.

@param hovercraft   to display message for. This message is only displayed if
                    this hovercraft's UI is displayed.
@param text         to display
*/
void GameInterface::displayMessage(eHovercraft hovercraft, std::string text, vec3 color)
{
    m_sMessages[hovercraft] = text;
    m_fMessageTimes[hovercraft] = MESSAGE_DURATION;
    m_sMessageColors[hovercraft] = color;
}

/*
    This updates the GameInterface stat values since last update.

    This should be called once per frame update when in game and while not
    paused.

    Units: seconds
*/
void GameInterface::updateOverride(float fFrameDeltaTime)
{
    if (m_iDisplayCount > 0)
    {
        updateGameTime(fFrameDeltaTime);
    }
}

void GameInterface::updateResumeCountdown(float fFrameDeltaTime)
{
    float tickTime = m_fResumeTime[m_iCurrentTick];

    if (tickTime <= 0 && m_iCurrentTick == 0) {
        // Leave early if done with countdown
        return;
    }

    tickTime -= fFrameDeltaTime;

    m_fResumeTime[m_iCurrentTick] = tickTime;
    
    // Update tick if time is exceeded
    if (tickTime <= 0)
    {
        m_iCurrentTick = FuncUtils::max(0, m_iCurrentTick - 1);
        cout << "tick: " << m_iCurrentTick << endl;
    }

}

void GameInterface::startResumeCountdown()
{
    for (int i = 0; i < COUNTDOWN_TICKS; ++i)
    {
        m_fResumeTime[i] = RESUME_TICK_DURATION;
    }
    m_iCurrentTick = COUNTDOWN_TICKS - 1;
}

/*
    Initialize all values to the start of a new game.
*/
void GameInterface::reinitialize(float gameTime)
{
    m_fGameTime = gameTime;
    m_bHasStartedMajorWarning = false;
    m_bHasStartedMinorWarning = false;

    for (int i = 0; i < MAX_HOVERCRAFT_COUNT; ++i)
    {
        m_fPowerupMessageTimes[i] = 0;
        m_fMessageTimes[i] = 0;
    }
    m_fNotificationTime = 0;
    startResumeCountdown();
}

/*
Renders the most recently updated state to the screen.
This this be called every render update, after the environment has been
rendered to ensure the UI is on top.
*/
void GameInterface::renderOverride()
{
    // renderText("Hello World!", 250.0f, 250.0f, 1.0f, vec3(1.0f));
    renderGameTime();
    renderScores();
    renderKillstreak();
    renderTeamBotScore();
    renderTeamPlayerScore();
    renderCooldowns();
    renderKillsAndDeaths();
    renderMessages();
    renderNotifications();
    renderResumeCountdown();
}

/*

*/
void GameInterface::updateGameTime(float fFrameDeltaTime)
{

    updateResumeCountdown(fFrameDeltaTime);
    m_fGameTime -= fFrameDeltaTime;
    if (m_fGameTime < 0)
    {
        m_fGameTime = 0;
    }

    for (int player = 0; player < m_iDisplayCount; player++)
    {
        m_fMessageTimes[player] -= fFrameDeltaTime;
        m_fScoreChangeTimes[player] -= fFrameDeltaTime;
        m_fPowerupMessageTimes[player] -= fFrameDeltaTime;
    }
    // TODO make sure time does not become negative, or if it does, it signifies
    // the end of the round. Not sure if its worth the cost to check.
    m_fNotificationTime -= fFrameDeltaTime;
}

/*
The time is formatted as

        minutes:seconds
*/
void GameInterface::renderGameTime()
{
    int secondsRemaining = static_cast<int>(m_fGameTime);
    vec3 color;
    if (secondsRemaining <= TIME_WARNING_MAJOR_START)
    {
        if (!m_bHasStartedMajorWarning)
        {
            startMajorTimeWarning();
        }
        color = TIME_WARNING_MAJOR_COLOR;
    }
    else if (secondsRemaining <= TIME_WARNING_MINOR_START)
    {
        if (!m_bHasStartedMinorWarning)
        {
            startMinorTimeWarning();
        }
        color = TIME_WARNING_MINOR_COLOR;
    } 
    else
    {
        color = TIME_COLOR;
    }
    renderText(FuncUtils::timeToString(secondsRemaining),
               m_vComponentCoordinates[COMPONENT_TIME].first,
               m_vComponentCoordinates[COMPONENT_TIME].second,
               TIME_SCALE, color);

}

/*
    The major time warning notifies all players of the remaining time, and begins
    the time countdown tick.
*/
void GameInterface::startMajorTimeWarning()
{
    m_bHasStartedMajorWarning = true;
    m_pSoundManager->stopEvent(SoundManager::eSoundEvent::SOUND_MUSIC_INGAME);
    m_pSoundManager->play(SoundManager::eSoundEvent::SOUND_UI_TIME_REMAINING_LOOP);
    displayNotification(NOTIFICATION_TIME_MAJOR);
}

/*
    The minor time warning notifies all players of the remaining time.
*/
void GameInterface::startMinorTimeWarning()
{
    m_bHasStartedMinorWarning = true;
    m_pSoundManager->play(SoundManager::eSoundEvent::SOUND_UI_TIME_REMAINING_WARNING);
    displayNotification(NOTIFICATION_TIME_MINOR);
}

/*
    Render messages for the focus hovercraft
*/
void GameInterface::renderMessages()
{
    if (m_fMessageTimes[m_eHovercraftFocus] > 0)
    {
        renderText(m_sMessages[m_eHovercraftFocus],
            m_vComponentCoordinates[COMPONENT_MESSAGE].first,
            m_vComponentCoordinates[COMPONENT_MESSAGE].second,
            MESSAGE_SCALE, m_sMessageColors[m_eHovercraftFocus]);
    }
    if (m_fScoreChangeTimes[m_eHovercraftFocus] > 0)
    {
        int scoreChange = GAME_STATS->get(m_eHovercraftFocus,
                                          GameStats::eHovercraftStat::SCORE_CHANGE);
        // cout << "GameInterface score change for " << m_eHovercraftFocus << ": " << scoreChange << endl;
        bool scoreIncreased = scoreChange >= 0;
        if (scoreChange != 0)
        {
            renderText((scoreIncreased ? "+" : "") + std::to_string(scoreChange) ,
                        m_vComponentCoordinates[COMPONENT_SCORE_CHANGE].first,
                        m_vComponentCoordinates[COMPONENT_SCORE_CHANGE].second,
                        SCORE_CHANGE_SCALE,
                        scoreIncreased ? SCORE_CHANGE_ADD_COLOR : SCORE_CHANGE_SUB_COLOR);
        }
    }
    if (m_fPowerupMessageTimes[m_eHovercraftFocus] > 0)
    {
        renderText(m_sPowerupMessages[m_eHovercraftFocus],
            m_vComponentCoordinates[COMPONENT_POWERUP].first,
            m_vComponentCoordinates[COMPONENT_POWERUP].second,
            MESSAGE_SCALE, MESSAGE_COLOR);
    }
}

/*
    Render the notification message to all players.
*/
void GameInterface::renderNotifications()
{
    for (int player = 0; player < MAX_HOVERCRAFT_COUNT; player++)
    {
        if (m_fNotificationTime > 0)
        {
            renderText(m_sNotification,
                m_vComponentCoordinates[COMPONENT_NOTIFICATION].first,
                m_vComponentCoordinates[COMPONENT_NOTIFICATION].second,
                MESSAGE_SCALE, m_sMessageColors[m_eHovercraftFocus]);
        }
    }
}

void GameInterface::renderResumeCountdown()
{
    // Check that there is resume time left.
    float tickTime = m_fResumeTime[m_iCurrentTick];

    if (tickTime >= 0)
    {
        string message;
        vec3 color;
        switch (m_iCurrentTick)
        {
        case 0:
            message = "GO";
            color = COUNTDOWN_GO_COLOR;
            break;
        case 1:
            message = "1";
            color = COUNTDOWN_1_COLOR;
            break;
        case 2:
            message = "2";
            color = COUNTDOWN_2_COLOR;
            break;
        case 3:
            message = "3";
            color = COUNTDOWN_3_COLOR;
            break;
        }

        renderText(message,
            m_vComponentCoordinates[COMPONENT_COUNTDOWN].first,
            m_vComponentCoordinates[COMPONENT_COUNTDOWN].second,
            MESSAGE_SCALE, color);
    }

}

void GameInterface::renderKillsAndDeaths()
{
    int kills = GAME_STATS->get(m_eHovercraftFocus,
            GameStats::eHovercraftStat::KILLS_TOTAL);
    int deaths = GAME_STATS->get(m_eHovercraftFocus,
            GameStats::eHovercraftStat::DEATHS_TOTAL);

    string kdr = "K/D   " + std::to_string(kills) + "/" + std::to_string(deaths);

    renderText(kdr,
        m_vComponentCoordinates[COMPONENT_KILL_DEATHS].first,
        m_vComponentCoordinates[COMPONENT_KILL_DEATHS].second,
        SCORE_SCALE, SCORE_COLOR);
}

void GameInterface::renderTeamPlayerScore()
{
    string score;
    string score2;
    vec3 color;
    vec3 color2;
    switch (GAME_STATS->getGameMode())
    {
    case GAMEMODE_TEAMS_AI_VS_PLAYERS:
        score = "Player team: " + to_string(GAME_STATS->get(GameStats::eGlobalStat::TEAM_PLAYER_SCORE));
        renderText(score,
           m_vComponentCoordinates[COMPONENT_TEAM_PLAYER_SCORE].first,
           m_vComponentCoordinates[COMPONENT_TEAM_PLAYER_SCORE].second,
           SCORE_SCALE,
           GAME_MANAGER->getHovercraftColor(m_eHovercraftFocus));
        break;
    case GAMEMODE_TEAMS_PLAYERS:
        switch (m_eHovercraftFocus)
        {
        case HOVERCRAFT_PLAYER_1:
        case HOVERCRAFT_PLAYER_2:
        score = "Your team: " + to_string(GAME_STATS->get(GameStats::eGlobalStat::TEAM_PLAYER_SCORE));
        score2 = "Enemy team: " + to_string(GAME_STATS->get(GameStats::eGlobalStat::TEAM2_PLAYER_SCORE));
        color = GAME_MANAGER->getHovercraftColor(HOVERCRAFT_PLAYER_1);
        color2 = GAME_MANAGER->getHovercraftColor(HOVERCRAFT_PLAYER_3);
            break;
        case HOVERCRAFT_PLAYER_3:
        case HOVERCRAFT_PLAYER_4:
        score = "Your team: " + to_string(GAME_STATS->get(GameStats::eGlobalStat::TEAM2_PLAYER_SCORE));
        score2 = "Enemy team: " + to_string(GAME_STATS->get(GameStats::eGlobalStat::TEAM_PLAYER_SCORE));
        color = GAME_MANAGER->getHovercraftColor(HOVERCRAFT_PLAYER_3);
        color2 = GAME_MANAGER->getHovercraftColor(HOVERCRAFT_PLAYER_1);
            break;
        }
        renderText(score,
           m_vComponentCoordinates[COMPONENT_TEAM_PLAYER_SCORE].first,
           m_vComponentCoordinates[COMPONENT_TEAM_PLAYER_SCORE].second,
           SCORE_SCALE,
           color);
        renderText(score2,
           m_vComponentCoordinates[COMPONENT_TEAM2_PLAYER_SCORE].first,
           m_vComponentCoordinates[COMPONENT_TEAM2_PLAYER_SCORE].second,
           SCORE_SCALE,
           color2);
        break;
    }
}

void GameInterface::renderTeamBotScore()
{
    string score;
    switch (GAME_STATS->getGameMode())
    {
    case GAMEMODE_TEAMS_AI_VS_PLAYERS:
    case GAMEMODE_TEAM_AI_SOLO_PLAYERS:
    case GAMEMODE_TEAMS_PLAYERS:
        if (GAME_STATS->getBotCount() > 0)
        {
            score = "Bot team: " + to_string(GAME_STATS->get(GameStats::eGlobalStat::TEAM_BOT_SCORE));
            renderText(score,
               m_vComponentCoordinates[COMPONENT_TEAM_BOT_SCORE].first,
               m_vComponentCoordinates[COMPONENT_TEAM_BOT_SCORE].second,
               SCORE_SCALE,
               GAME_MANAGER->getHovercraftColor(HOVERCRAFT_BOT_1));
        }
        break;
    }
}

void GameInterface::renderKillstreak()
{
    renderText("Killstreak: ",
        m_vComponentCoordinates[COMPONENT_KILLSTREAK].first,
        m_vComponentCoordinates[COMPONENT_KILLSTREAK].second,
        SCORE_SCALE,
        COLOR_WHITE);
    int killstreak = GAME_STATS->get(m_eHovercraftFocus, GameStats::eHovercraftStat::KILLSTREAK_CURRENT);
    string message;
    vec3 color;
    if (killstreak > 0)
    {
        message = "+" + to_string(killstreak);
        color = COLOR_GREEN;
    }
    else if (killstreak < 0)
    {
        message = to_string(killstreak);
        color = COLOR_RED;
    }
    else
    {
        message = to_string(killstreak);
        color = COLOR_WHITE;
    }
    renderText(message,
        m_vComponentCoordinates[COMPONENT_KILLSTREAK].first + 200,
        m_vComponentCoordinates[COMPONENT_KILLSTREAK].second,
        SCORE_SCALE,
        color);
}

void GameInterface::renderScores()
{
    string score = std::to_string(GAME_STATS->get(m_eHovercraftFocus,
                        GameStats::eHovercraftStat::SCORE_CURRENT));
    renderText(m_eHovercraftToString.at(m_eHovercraftFocus) + ": " + score,
               m_vComponentCoordinates[COMPONENT_SCORE].first,
               m_vComponentCoordinates[COMPONENT_SCORE].second,
               SCORE_SCALE,
               GAME_MANAGER->getHovercraftColor(m_eHovercraftFocus));
}

void GameInterface::updateCooldowns()
{
}

void GameInterface::renderCooldowns()
{
    // TODO put this in the proper place, font, scale etc.
    // This formatting is all temporary
    // 0 - 100
    HovercraftEntity* hovercraft = m_pEntityMngr->getHovercraft(m_eHovercraftFocus);
    float* cooldowns = hovercraft->getCooldowns();
    float trailPercent = hovercraft->getTrailGaugePercent();
    std::string trailPercentString = std::to_string((int) (trailPercent * 100));
    vec3 trailColor = trailPercent == 1.0 ?
        COLOR_READY : trailPercent == 0.0 ?
        COLOR_NOT_READY : COLOR_MID_READY;
    renderText("Flame: " + trailPercentString + "%",
               m_vComponentCoordinates[COMPONENT_TRAIL].first,
               m_vComponentCoordinates[COMPONENT_TRAIL].second,
               TRAIL_SCALE,
               trailColor);
    renderCooldown("Rocket",
                   eCooldown::COOLDOWN_ROCKET,
                   cooldowns,
                   m_vComponentCoordinates[COMPONENT_ROCKET].first,
                   m_vComponentCoordinates[COMPONENT_ROCKET].second,
                   ROCKET_SCALE);
    renderCooldown("Spikes" + std::string(hovercraft->hasSpikesActivated() ? " enabled" : ""),
                   eCooldown::COOLDOWN_SPIKES,
                   cooldowns,
                   m_vComponentCoordinates[COMPONENT_SPIKES].first,
                   m_vComponentCoordinates[COMPONENT_SPIKES].second,
                   SPIKES_SCALE);
    renderCharges(cooldowns, hovercraft);

    //  renderImage(IMAGE_TRAIL, 0, 0, 10);
}

/*
    Render an ability cooldown label.

    @param label        name of the cooldown
    @param cooldown     of ability, to determine the cooldown value to retrieve from cooldowns
    @param cooldowns    of all ability cooldowns
    @param x            x-coordinate to place cooldown label
    @param y            y-coordinate to place cooldown label
    @param scale        of the label
*/
void GameInterface::renderCooldown(std::string label,
                                   eCooldown cooldown,
                                   float* cooldowns,
                                   GLfloat x, GLfloat y, GLfloat scale)
{
    bool isReady = cooldowns[cooldown] == 0;
    std::string cooldownString = isReady ?
        COOLDOWN_READY :
        FuncUtils::toString(cooldowns[cooldown], COOLDOWN_DECIMAL_PLACES) + "s";
    vec3 color = isReady ? COLOR_READY : COLOR_NOT_READY;
    renderText(label + ": " + cooldownString, x, y, scale, color);
}

// Right now only dones for dashes
// Renders dash with it's charges and recharge cooldowns
// @TODO cooldowns unused
void GameInterface::renderCharges(float* cooldowns, HovercraftEntity* hovercraft)
{
    bool canDash = hovercraft->canDash();
    bool isFull = hovercraft->hasMaxDashCharges();
    std::string cooldownString = isFull ?
        COOLDOWN_READY :
        FuncUtils::toString(hovercraft->getDashRecharge(), COOLDOWN_DECIMAL_PLACES) + "s";
    vec3 color = isFull ? COLOR_READY : canDash ? COLOR_MID_READY : COLOR_NOT_READY;
    std::string labelWithCharges = "Dash (" + std::to_string(hovercraft->getDashCharges()) + "): ";
    renderText(labelWithCharges + cooldownString,
               m_vComponentCoordinates[COMPONENT_DASH].first,
               m_vComponentCoordinates[COMPONENT_DASH].second,
               DASH_SCALE,
               color);

}


/*
    @EvanQuan : Not Implemented
*/
void GameInterface::renderComponent(eUIComponent component, GLfloat scale, vec3 color)
{
    /* TODO */
}

/*
Set the number of UIs to display. This determines how many hovercrafts the
interface should update for.

Values:
    0 - No UIs are displayed. This should be done in the main menu (not in game).
    1 - 1 player. This will display across the whole window.
    2 - 2 player. Each UI will display across 1/4 of the window.
    3 - 3 player. Each UI will display across 1/4 of the window.
    4 - 4 player. Each UI will display across 1/4 of the window.
*/
void GameInterface::setDisplayCount(int count)
{
    m_iDisplayCount = FuncUtils::bound(count, DISPLAY_COUNT_MIN, DISPLAY_COUNT_MAX);
}

/*
    Display the specified hovercraft's UI information.

    @param hovercraft   to display
*/
void GameInterface::setFocus(eHovercraft hovercraft)
{
    m_eHovercraftFocus = hovercraft;
}

