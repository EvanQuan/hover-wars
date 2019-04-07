#pragma once
#include "UserInterface/UserInterface.h"

#define IMAGE_ROCKET "textures/hud/rocket.png"
#define IMAGE_TRAIL "textures/hud/trail.png"
#define IMAGE_EXPLOSION "textures/hud/explosion.png"
#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4

// Forward Declaration
class EntityManager;
class HovercraftEntity;
class SoundManager;


/*
An interface to all user interface features.

Retrieves its values from GameStats

- Display text, images
*/
class GameInterface : public UserInterface
{
public:

    enum eKillMessage
    {
        KILL_MESSAGE_FIRST_BLOOD = 0,
        KILL_MESSAGE_DOMINATION,
        KILL_MESSAGE_REVENGE,
        KILL_MESSAGE_KILLSTREAK,
        KILL_MESSAGE_KILL,
        // @Deprecated
        KILL_MESSAGE_NEW_LEADER,
    };

    enum eNotification
    {
        NOTIFICATION_TIME_MINOR, 
        NOTIFICATION_TIME_MAJOR, 
        NOTIFICATION_3, 
        NOTIFICATION_2, 
        NOTIFICATION_1, 
        NOTIFICATION_GO, 
    };

    static GameInterface* getInstance(int iWidth, int iHeight);

    ~GameInterface();

    /*
    This visually updates the GameInterface to all value changes since last update.
    
    Under the scenes, this retrieves all needed the values from GameStats and
    displays them. This is why it does not need a time value in order to determine
    time-sensitive information such as cooldown and game time.
    @TODO this may change later on.
    
    This should be called once per frame update.
    
    */
    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

    void setDisplayCount(int count);

    void setFocus(eHovercraft hovercraft);

    void displayKillMessage(eHovercraft attacker, eHovercraft hit, eKillMessage message);
    // Send a message to everyone.
    void displayNotification(eNotification message);
    void displayPowerup(eHovercraft hovercraft, ePowerup powerup);

    // Display debug message
    // Set message to "" to disable debug message
    void displayDebug(std::string message);

private:

    // Used for m_vComponentScaling and m_vComponentCoordinates
    enum eUIComponent
    {
        COMPONENT_TIME = 0,
        COMPONENT_TRAIL,
        COMPONENT_SPIKES,
        COMPONENT_ROCKET,
        COMPONENT_DASH,
        COMPONENT_SCORE,
        COMPONENT_SCORE_CHANGE,
        COMPONENT_MESSAGE,
        COMPONENT_POWERUP,
        COMPONENT_NOTIFICATION,
        COMPONENT_COUNT
    };

    GameInterface();                                        // Default Constructor
    GameInterface(const GameInterface* pCopy);              // Default Copy Constructor
    GameInterface& operator=(const GameInterface* pCopy);   // Assignment Operator.
    static GameInterface* m_pInstance;
    EntityManager* m_pEntityMngr;
    SoundManager* m_pSoundManager;

    void displayMessage(eHovercraft hovercraft, std::string text);
    /*
    Other classes should not be able to directly tell the UI to render text or
    images. Instead, the UI gathers the necessary information from other
    classes, such as GameStats, where it decides what text and images needs to
    be updated during its update() call.
    */
    void renderComponent(eUIComponent component, GLfloat scale, vec3 color);

    // Game Time
    void updateGameTime(float fSecondsSinceLastUpdate);
    void renderGameTime();
    void startMajorTimeWarning();
    void startMinorTimeWarning();
    
    // Message
    void renderMessages();

    // Score
    void renderScores();
    
    void renderNotifications();

    // Cooldowns
    void updateCooldowns();
    void renderCooldowns();
    void renderCooldown(std::string label, eCooldown cooldown, float* cooldowns, GLfloat x, GLfloat y, GLfloat scale);
    void renderCharges(float* cooldowns, HovercraftEntity* hovercraft);

    /*
    NOTE: this may need to change in the future.

    Unit : seconds
    */
    float m_fGameTime;
    bool m_bHasStartedMajorWarning;
    bool m_bHasStartedMinorWarning;

    /*
    Tracks how long the message has been displayed for

    Unit : seconds
    */
    std::string m_sPowerupMessages[MAX_HOVERCRAFT_COUNT];
    std::string m_sMessages[MAX_HOVERCRAFT_COUNT];
    float m_fPowerupMessageTimes[MAX_HOVERCRAFT_COUNT];
    float m_fMessageTimes[MAX_HOVERCRAFT_COUNT];

    /*
        Notifications are singular messages shared amongst all players.
        Their location is different than messages to prevent meessagee overlap.
    */
    std::string m_sNotification;
    float m_fNotificationTime;

    /*
    Score updates appear temporarily just as messages are

    Unit : seconds
    */
    float m_fScoreChangeTimes[MAX_HOVERCRAFT_COUNT];

    int m_iDisplayCount;

    eHovercraft m_eHovercraftFocus;

    const unordered_map<eHovercraft, std::string> m_eHovercraftToString =
    {
        {HOVERCRAFT_BOT_1, "Alfa Bot"},
        {HOVERCRAFT_BOT_2, "Bravo Bot"},
        {HOVERCRAFT_BOT_3, "Charlie Bot"},
        {HOVERCRAFT_BOT_4, "Delta Bot"},
        {HOVERCRAFT_PLAYER_1, "Player 1"},
        {HOVERCRAFT_PLAYER_2, "Player 2"},
        {HOVERCRAFT_PLAYER_3, "Player 3"},
        {HOVERCRAFT_PLAYER_4, "Player 4"},
    };

};
