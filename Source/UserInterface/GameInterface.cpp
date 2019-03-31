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


// Singleton instance
GameInterface* GameInterface::m_pInstance = nullptr;

GameInterface::GameInterface(int iWidth, int iHeight) : UserInterface(iWidth, iHeight,
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
        {0.45f, 0.33f}
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
        {0.0f, 0.0f}

    }
)
{
    GAME_MANAGER->addInterface(this);
    setDisplayCount(1);
    debugMessage = "";
    m_pEntityMngr = ENTITY_MANAGER;
}

GameInterface* GameInterface::getInstance(int iWidth, int iHeight)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new GameInterface(iWidth, iHeight);
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
void GameInterface::displayMessage(eHovercraft attacker, eHovercraft hit, eKillMessage message)
{
    bool attackerIsPlayer = GAME_STATS->isPlayer(attacker);
    bool hitIsPlayer = GAME_STATS->isPlayer(hit);
    bool playerInvolved = attackerIsPlayer || hitIsPlayer;
    switch (message)
    {
    case KILL_MESSAGE_DOMINATION:
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_DOMINATION, playerInvolved);
        displayMessage(attacker, "You are now dominating " + m_eHovercraftToString.at(hit));
        displayMessage(hit, m_eHovercraftToString.at(attacker) + " is now dominating you");
        break;
    case KILL_MESSAGE_FIRST_BLOOD:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_KILL_FIRST_BLOOD);
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
        // displayMessage(attacker, "You got first blood against " + m_eHovercraftToString.at(hit));
        // displayMessage(hit, m_eHovercraftToString.at(attacker) + " got first blood against you");
        break;
    case KILL_MESSAGE_REVENGE:
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_REVENGE, playerInvolved);
        displayMessage(attacker, "You got revenge from " + m_eHovercraftToString.at(hit));
        displayMessage(hit, m_eHovercraftToString.at(attacker) + " got revenge from you");
        break;
    case KILL_MESSAGE_KILLSTREAK:
        SOUND_MANAGER->play(SoundManager::SOUND_KILL_STREAK, playerInvolved);
        displayMessage(attacker, "You have a killstreak of " + std::to_string(GAME_STATS->get(attacker, GameStats::eHovercraftStat::KILLSTREAK_CURRENT)));
        break;
    case KILL_MESSAGE_KILL:
        m_fScoreChangeTimes[attacker] = SCORE_CHANGE_DURATION;
        m_fScoreChangeTimes[hit] = SCORE_CHANGE_DURATION;
        break;
    }
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

void GameInterface::reinitialize(float gameTime)
{
    m_fGameTime = gameTime;
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
}

/*
The time is formatted as

        minutes:seconds

For now, the game time is going up from 0. Later this should count down.
*/
void GameInterface::renderGameTime()
{
    renderText(FuncUtils::timeToString(static_cast<int>(m_fGameTime)),
               m_vComponentCoordinates[COMPONENT_TIME].first,
               m_vComponentCoordinates[COMPONENT_TIME].second,
               TIME_SCALE, TIME_COLOR);

}

/*
    Includes powerup messages
*/
void GameInterface::renderMessages()
{
    for (int player = 0; player < m_iDisplayCount; player++)
    {
        if (m_fMessageTimes[player] > 0)
        {
            renderText(m_sMessages[player],
                m_vComponentCoordinates[COMPONENT_MESSAGE].first,
                m_vComponentCoordinates[COMPONENT_MESSAGE].second,
                MESSAGE_SCALE, MESSAGE_COLOR);
        }
        if (m_fScoreChangeTimes[player] > 0)
        {
            int scoreChange = GAME_STATS->get(static_cast<eHovercraft>(player),
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
        if (m_fPowerupMessageTimes[player] > 0)
        {
            renderText(m_sPowerupMessages[player],
                m_vComponentCoordinates[COMPONENT_POWERUP].first,
                m_vComponentCoordinates[COMPONENT_POWERUP].second,
                MESSAGE_SCALE, MESSAGE_COLOR);
        }
    }

    if (!debugMessage.empty())
    {
        renderText(debugMessage, debugWidth, debugHeight, 1.0f, COLOR_WHITE);
    }
}

void GameInterface::displayDebug(std::string message)
{
    debugMessage = message;
}

/*
Update the scores for all display count players
*/
void GameInterface::updateScores()
{
    for (int player = 0; player < m_iDisplayCount; player++)
    {
        updateScore(static_cast<eHovercraft>(player),
                    GAME_STATS->get(static_cast<eHovercraft>(player),
                    GameStats::SCORE_CURRENT));
    }
}

// @EvanQuan : Not Implemented, "hovecraft?" misspelled?
void GameInterface::updateScore(eHovercraft hovecraft, int score)
{
    
    // cout << "Player " << (player + 1) << " score: " << score << endl;
}

void GameInterface::renderScores()
{
    // TODO put this in the proper place, font, scale etc.
    // Ad hoc for single player
    std::string score = std::to_string(GAME_STATS->get(HOVERCRAFT_PLAYER_1,
                        GameStats::eHovercraftStat::SCORE_CURRENT));
    renderText("Score: " + score,
               m_vComponentCoordinates[COMPONENT_SCORE].first,
               m_vComponentCoordinates[COMPONENT_SCORE].second,
               SCORE_SCALE, SCORE_COLOR);
}

void GameInterface::updateCooldowns()
{
}

void GameInterface::renderCooldowns()
{
    // TODO put this in the proper place, font, scale etc.
    // This formatting is all temporary
    // 0 - 100
    // Ad hoc for single player
    HovercraftEntity* hovercraft = m_pEntityMngr->getHovercraft(HOVERCRAFT_PLAYER_1);
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
Set the number of UIs to display.
Values:
    0 - No UIs are displayed. This should be done in the main menu (not in game).
    1 - 1 player. This will display across the whole window.
    2 - 2 player. Each UI will display across 1/4 of the window.
    3 - 3 player. Each UI will display across 1/4 of the window.
    4 - 4 player. Each UI will display across 1/4 of the window.

@TODO set count for main menu or pause menu
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

    initializeGameInterface();
}

/*
Initialize all aspects of the UI according to the current display count. This
should be done at the start of every game, or if the game resets.
*/
void GameInterface::initializeGameInterface()
{
    initializeScores();
    initializeCooldowns();
}

/*
Initialize the scores of all players to 0
*/
void GameInterface::initializeScores()
{
    for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
    {
    }
}

/*
Initialize the cool down counts for all abilities
*/
void GameInterface::initializeCooldowns()
{
    for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
    {
        // TODO
    }
}

/*
Set a specified player's score to the specified value.
*/
void GameInterface::setScore(int joystickID, int score)
{
}

/*
Initialize all UI components according to the current value of m_iDisplayCount
*/
void initializeDisplayCount()
{
    // TODO
}
