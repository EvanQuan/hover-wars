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
#define TIME_WARNING_SOUND_START TIME_WARNING_MAJOR + 2

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
        {0.36f, 0.65f},
        // 8 Powerup
        {0.45f, 0.33f}, 
        // 9 Notification
        {0.36f, 0.65f},
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
        {0.00f, 0.00f},
        // 7 Message
        {0.00f, 0.0f},
        // 8 Powerup
        {0.0f, 0.0f},
        // 9 Notification
        {0.0f, -10.0f},

    }
)
{
    GAME_MANAGER->addInterface(this);
    setDisplayCount(1);
    m_eHovercraftFocus = HOVERCRAFT_PLAYER_1;
    debugMessage = "";
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
        displayMessage(attacker, "You are now dominating " + m_eHovercraftToString.at(hit));
        displayMessage(hit, m_eHovercraftToString.at(attacker) + " is now dominating you");
        break;
    case KILL_MESSAGE_FIRST_BLOOD:
        m_pSoundManager->play(SoundManager::eSoundEvent::SOUND_KILL_FIRST_BLOOD);
        // Announce first blood to everyone
        for (int player = 0, playerCount = GAME_STATS->getPlayerCount();
             player < playerCount;
             player++)
        {
            string attackerName = attacker == player ? "You" : m_eHovercraftToString.at(attacker);
            string hitName = hit == player ? "you" : m_eHovercraftToString.at(hit);
            displayMessage(static_cast<eHovercraft>(player),
                attackerName + " got first blood against " + hitName);
        }
        // displayKillMessage(attacker, "You got first blood against " + m_eHovercraftToString.at(hit));
        // displayKillMessage(hit, m_eHovercraftToString.at(attacker) + " got first blood against you");
        break;
    case KILL_MESSAGE_REVENGE:
        m_pSoundManager->play(SoundManager::SOUND_KILL_REVENGE, playerInvolved);
        displayMessage(attacker, "You got revenge from " + m_eHovercraftToString.at(hit));
        displayMessage(hit, m_eHovercraftToString.at(attacker) + " got revenge from you");
        break;
    case KILL_MESSAGE_KILLSTREAK:
        m_pSoundManager->play(SoundManager::SOUND_KILL_STREAK, playerInvolved);
        displayMessage(attacker, "You have a killstreak of "
            + std::to_string(GAME_STATS->get(attacker, GameStats::eHovercraftStat::KILLSTREAK_CURRENT)));
        break;
    case KILL_MESSAGE_KILL:
        m_fScoreChangeTimes[attacker] = SCORE_CHANGE_DURATION;
        m_fScoreChangeTimes[hit] = SCORE_CHANGE_DURATION;
        break;
    case KILL_MESSAGE_NEW_LEADER:
        m_pSoundManager->play(SoundManager::SOUND_UI_NEW_LEADER);
        for (int player = 0, playerCount = GAME_STATS->getPlayerCount();
            player < playerCount;
            player++)
        {
            string newLeaderName = attacker == player ?
                "You are" : m_eHovercraftToString.at(attacker) + " is";
            displayMessage(static_cast<eHovercraft>(player),
                newLeaderName + " now in the lead");
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
    case NOTIFICATION_3:
        m_sNotification = "3";
        break;
    case NOTIFICATION_2:
        m_sNotification = "2";
        break;
    case NOTIFICATION_1:
        m_sNotification = "1";
        break;
    case NOTIFICATION_GO:
        m_sNotification = "GO";
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
void GameInterface::displayMessage(eHovercraft hovercraft, std::string text)
{
    m_sMessages[hovercraft] = text;
    m_fMessageTimes[hovercraft] = MESSAGE_DURATION;
}

/*
This visually updates the GameInterface to all value changes since last update.

Under the scenes, this retrieves all needed the values from GameStats and
displays them. This is why it does not need a time value in order to determine
time-sensitive information such as cooldown and game time.
@TODO this may change later on.

This should be called once per frame update.

*/
void GameInterface::update(float fSecondsSinceLastUpdate)
{
    if (m_iDisplayCount > 0)
    {
        // system("CLS");

        updateGameTime(fSecondsSinceLastUpdate);
        // updateScores();
        // updateCooldowns();
    }
}

/*
    Initialize all values to the start of a new game.
*/
void GameInterface::reinitialize(float gameTime)
{
    m_fGameTime = gameTime;
    m_bHasStartedWarning = false;

    for (int i = 0; i < MAX_HOVERCRAFT_COUNT; ++i)
    {
        m_fPowerupMessageTimes[i] = 0;
        m_fMessageTimes[i] = 0;
    }
    m_fNotificationTime = 0;
}

/*
Renders the most recently updated state to the screen.
This this be called every render update, after the environment has been
rendered to ensure the UI is on top.
*/
void GameInterface::render()
{
    // renderText("Hello World!", 250.0f, 250.0f, 1.0f, vec3(1.0f));
    renderGameTime();
    renderScores();
    renderCooldowns();
    renderMessages();
    renderNotifications();
}

/*

*/
void GameInterface::updateGameTime(float fSecondsSinceLastUpdate)
{

    m_fGameTime -= fSecondsSinceLastUpdate;
    if (m_fGameTime < 0)
    {
        m_fGameTime = 0;
    }

    for (int player = 0; player < m_iDisplayCount; player++)
    {
        m_fMessageTimes[player] -= fSecondsSinceLastUpdate;
        m_fScoreChangeTimes[player] -= fSecondsSinceLastUpdate;
        m_fPowerupMessageTimes[player] -= fSecondsSinceLastUpdate;
    }
    // TODO make sure time does not become negative, or if it does, it signifies
    // the end of the round. Not sure if its worth the cost to check.
    m_fNotificationTime -= fSecondsSinceLastUpdate;
}

/*
The time is formatted as

        minutes:seconds
*/
void GameInterface::renderGameTime()
{
    int secondsRemaining = static_cast<int>(m_fGameTime);
    if (secondsRemaining <= TIME_WARNING_SOUND_START && !m_bHasStartedWarning)
    {
        m_bHasStartedWarning = true;
        m_pSoundManager->stopEvent(SoundManager::eSoundEvent::MUSIC_INGAME);
        m_pSoundManager->play(SoundManager::eSoundEvent::SOUND_UI_TIME_REMAINING_LOOP);
    }
    vec3 color = secondsRemaining <= TIME_WARNING_MAJOR ?TIME_WARNING_MAJOR_COLOR
        : secondsRemaining <= TIME_WARNING_MINOR ? TIME_WARNING_MINOR_COLOR : TIME_COLOR;
    renderText(FuncUtils::timeToString(secondsRemaining),
               m_vComponentCoordinates[COMPONENT_TIME].first,
               m_vComponentCoordinates[COMPONENT_TIME].second,
               TIME_SCALE, color);

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
            MESSAGE_SCALE, MESSAGE_COLOR);
    }
    if (m_fScoreChangeTimes[m_eHovercraftFocus] > 0)
    {
        int scoreChange = GAME_STATS->get(m_eHovercraftFocus,
                                          GameStats::eHovercraftStat::SCORE_CHANGE);
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

    // @TODO remove this?
    if (!debugMessage.empty())
    {
        renderText(debugMessage, debugWidth, debugHeight, 1.0f, COLOR_WHITE);
    }
}

void GameInterface::renderNotifications()
{
    for (int player = 0; player < MAX_HOVERCRAFT_COUNT; player++)
    {
        renderText(m_sNotification,
            m_vComponentCoordinates[COMPONENT_NOTIFICATION].first,
            m_vComponentCoordinates[COMPONENT_NOTIFICATION].second,
            MESSAGE_SCALE, MESSAGE_COLOR);
    }
}

void GameInterface::displayDebug(std::string message)
{
    debugMessage = message;
}

void GameInterface::renderScores()
{
    // TODO put this in the proper place, font, scale etc.
    // Ad hoc for single player
    for (int player = 0; player < MAX_HOVERCRAFT_COUNT; player++)
    {
        eHovercraft hovercraft = static_cast<eHovercraft>(player);
        std::string score = std::to_string(GAME_STATS->get(hovercraft,
                            GameStats::eHovercraftStat::SCORE_CURRENT));
        renderText("Score: " + score,
                   m_vComponentCoordinates[COMPONENT_SCORE].first,
                   m_vComponentCoordinates[COMPONENT_SCORE].second,
                   SCORE_SCALE, SCORE_COLOR);
    }
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
    if (DISPLAY_COUNT_MIN > count)
    {
        count = DISPLAY_COUNT_MIN;
    }
    else if (DISPLAY_COUNT_MAX < count)
    {
        count = DISPLAY_COUNT_MAX;
    }
    m_iDisplayCount = count;
}

/*
    Display the specified hovercraft's UI information.

    @param hovercraft   to display
*/
void GameInterface::setFocus(eHovercraft hovercraft)
{
    m_eHovercraftFocus = hovercraft;
}

