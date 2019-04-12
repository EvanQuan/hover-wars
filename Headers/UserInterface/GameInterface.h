#pragma once
#include "UserInterface/UserInterface.h"

#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4

#define COUNTDOWN_TICKS 4

// Forward Declaration
class EntityManager;
class HovercraftEntity;
class SoundManager;
class GameStats;


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
        KILL_MESSAGE_NEW_LEADER,
    };

    enum eNotification
    {
        NOTIFICATION_TIME_MINOR, 
        NOTIFICATION_TIME_MAJOR, 
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
    void updateOverride(float fFrameDeltaTime);
    void updateResumeCountdown(float fFrameDeltaTime);

    void startResumeCountdown();

    void reinitialize(float gameTime);

    void renderOverride();

    void setDisplayCount(int count);

    void setFocus(eHovercraft hovercraft);

    void displayKillMessage(eHovercraft attacker, eHovercraft hit, eKillMessage message);
    // Send a message to everyone.
    void displayNotification(eNotification message);
    void displayPowerup(eHovercraft hovercraft, ePowerup powerup);

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
        COMPONENT_COUNTDOWN,
        COMPONENT_KILL_DEATHS,
        COMPONENT_TEAM_BOT_SCORE,
        COMPONENT_TEAM_PLAYER_SCORE,
        COMPONENT_TEAM2_PLAYER_SCORE,
        COMPONENT_KILLSTREAK,
        COMPONENT_COUNT
    };

    /*
        Resume count down notifications display in their own location to
        prevent message or notification overlap.
    */
    enum eResumeCountdown
    {
        RESUME_3, 
        RESUME_2, 
        RESUME_1, 
        RESUME_GO, 
    };

    GameInterface();                                        // Default Constructor
    GameInterface(const GameInterface* pCopy);              // Default Copy Constructor
    GameInterface& operator=(const GameInterface* pCopy);   // Assignment Operator.
    static GameInterface* m_pInstance;
    EntityManager* m_pEntityMngr;
    SoundManager* m_pSoundManager;
    GameStats  *m_pGameStats;

    void displayMessage(eHovercraft hovercraft, std::string text, vec3 color);
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
    void renderResumeCountdown();
    void renderKillsAndDeaths();

    void renderTeamPlayerScore();
    void renderTeamBotScore();

    void renderKillstreak();

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
    string m_sPowerupMessages[MAX_HOVERCRAFT_COUNT];
    string m_sMessages[MAX_HOVERCRAFT_COUNT];
    vec3 m_sMessageColors[MAX_HOVERCRAFT_COUNT];
    float m_fPowerupMessageTimes[MAX_HOVERCRAFT_COUNT];
    float m_fMessageTimes[MAX_HOVERCRAFT_COUNT];

    /*
        Notifications are singular messages shared amongst all players.
        Their location is different than messages to prevent meessagee overlap.
    */
    string m_sNotification;
    float m_fNotificationTime;

    // The message for the current countdown tick 
    string m_sResumeMessage;
    // The time for the entire resume countdown
    float m_fResumeTime[COUNTDOWN_TICKS];
    int m_iCurrentTick;

    /*
    Score updates appear temporarily just as messages are

    Unit : seconds
    */
    float m_fScoreChangeTimes[MAX_HOVERCRAFT_COUNT];

    int m_iDisplayCount;

    eHovercraft m_eHovercraftFocus;

};
