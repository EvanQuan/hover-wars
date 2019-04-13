#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityHeaders/Camera.h"
#include <time.h>

// Forward Declarations
class AIManager;
class EntityManager;
class CommandHandler;
class ShaderManager;
class PromptInterface;
class UserInterface;
class GameInterface;
class GameStats;
class Texture;
class SoundManager;

// Class: Game Manager
// Purpose: Manages Game States and handles initialization of a level and handling of
//      Main menu interfacing
// Written by: James Coté
class GameManager final
{
public:
    static GameManager* getInstance(GLFWwindow *rWindow);
    static GameManager* getInstance();
    ~GameManager();
    
    // Graphics Application
    bool initialize();
    void startRendering();
    void initializeNewGame(unsigned int playerCount,
                           unsigned int botCount,
                           eBotDifficulty botDifficulty,
                           float gameTime,
                           eGameMode gameMode,
                           unsigned int mapNumber,
                           bool scoreLossEnabled);
    void resetTime() { m_pTimer.resetTimer(); }

    // Window Width and Height Getters and Setters
    int getWidth() { return m_iWidth; }
    int getHeight() { return m_iHeight; }
    void resizeWindow(int iWidth, int iHeight);

    // Camera Functions 
    //void snapShotCamera();
    void rotateCamera(vec2 pDelta);
    void zoomCamera(float fDelta);
    void intersectPlane(float fX, float fY);

    eHovercraft getKeyboardHovercraft() const { return m_eKeyboardHovercraft; }

    bool isPaused() const { return m_bPaused; }
    void setPaused(bool paused);

    void addInterface(UserInterface* ui);
    void setCurrentInterface(PromptInterface* ui);

    // Automatically called at end of game if time runs out, or if ended thrugh
    // pause menu
    void endGame();

    void flagWindowToClose() { glfwSetWindowShouldClose(m_pWindow, GL_TRUE); }

    PromptInterface* getMenuInterface() const { return m_pMenuInterface; }

    vec3 getPlayerColor(eHovercraft player) const { return (int)m_vPlayerColors.size() > (int)player ? m_vPlayerColors.at(player) : vec3(1.0f); }
    vec3 getBotColor(eHovercraft bot) const { return m_vBotColors.empty() ? vec3(1.0f) : m_vBotColors.at(bot - MAX_PLAYER_COUNT); }
    vec3 getHovercraftColor(eHovercraft hovercraft) const { return hovercraft <= HOVERCRAFT_PLAYER_4 ? getPlayerColor(hovercraft) : getBotColor(hovercraft); }

private:
    // For Singleton Implementation
    GameManager(GLFWwindow* rWindow); 
    GameManager(const GameManager* pCopy);
    static GameManager* m_pInstance;

    // Rendering
    bool renderGraphics();
    void drawScene();
    void renderSplitScreen();

    void updateEnvironment();

    // Initializing a new game
    void calculateScreenDimensions(unsigned int playerCount);
    void spawnHovercrafts(unsigned int playerCount, unsigned int botCount, eGameMode aiType);
    void selectColors(vector<vec3>* vReturnColors, vector<vec3>* vColorPool, unsigned int iNumPicks, bool bTeam);
    void spawnPlayers(unsigned int playerCount, const vector<vec3> &colors);
    void spawnBots(unsigned int botCount, const vector<vec3> &colors);
    void setupMapData();

    // Time
    void updateTime();
    void checkIfStartedGameOver();
    void updateInGame();
    void checkIfShouldEndGame();
    void startResumeCountdown();
    void resumeGame();
    void pauseGame();
    void checkIfShouldUpdateGameTime(float frameDeltaTime);
    void checkIfShouldResumeGame(float frameDeltaTime);

    // Split Screen Rendering variables
    GLuint m_iVertexArray, m_iVertexBuffer;
    struct sRenderBlock
    {
        GLuint iRenderBuffer, iFrameBuffer;
        Texture* pColorBuffer[2];
        struct sPingPongBuffer
        {
            GLuint iFBO;
            Texture* pBuffer;
        } pPingPongBuffers[2];
    };
    vector< sRenderBlock > m_pFrameBufferTextures;
    void generateSplitScreen(unsigned int iPlayer);
    void cleanupFrameBuffers();
    void generateFrameBuffer(unsigned int iPlayer);

    // Blur Rendering variables
    GLuint m_iBlurVAO, m_iBlurVBO;
    void blurBloomBuffer(unsigned int iScreen);

    // Map Rendering variables
    vector< vec3 > m_vPositions, m_vColors;
    GLuint m_iMapVAO, m_iMapVBO;
    void renderMap();

    // Screen Height and Width
    int m_iWidth, m_iSplitWidth;
    int m_iHeight, m_iSplitHeight;

    // Window Reference
    GLFWwindow* m_pWindow;

    // Update Variables
    /*
        The time since graphics have been initialized.
        This is tracked to ensure that all values update correctly for every
        frame update.

        Unit: seconds
    */
    duration<float> m_fFrameTime;
    /*
        This depends the rate at which the window is re-rendered.
        Since it is costly to re-render the window, it is locked to re-render
        at a fixed rate, even if the enviroment updates at a faster rate.

        Unit: seconds
    */
    duration<float> m_fMaxDeltaTime;

    /*
        Difference in time from this frame and the last.
        Needed for updating all the entities every frame.

        This is only needed for the EntityManager.
    */
    duration<double> m_fFrameDeltaTimePrecise;

    /*
        Difference in time from this frame and the last.
        Needed for updating all the entities every frame.

        Do not confuse this with EntityManager's delta time, which is much
        smaller since it updates more frequently than every frame update.

        Unit: seconds
    */
    float m_fFrameDeltaTime;

    // Manager Pointers
    EntityManager*      m_pEntityManager;
    ShaderManager*      m_pShaderManager;
    CommandHandler*     m_pCommandHandler;
    AIManager*          m_pAIManager;
    GameTime            m_pTimer;
    GameStats*          m_pGameStats;
    PhysicsManager*     m_pPhysicsManager;
    PromptInterface*      m_pMenuInterface;
    GameInterface*      m_pGameInterface;
    vector<UserInterface*> m_vInterfaceInstances;
    SoundManager*       m_pSoundManager;

    void setKeyboardHovercraft(int playerCount);
    // The keyboard corresponds to its own hovercraft
    // which might be shared with a joystick
    eHovercraft m_eKeyboardHovercraft;

    void updateGameTime(float frameDeltaTime);
    /*
        Tracks the time of a given game. Specified by the user in the
        PregameMenu, and counts down at the start of the game (after
        initialization).

        Unit: seconds
    */
    float m_fGameTime;
    /*
        After the user resumes the game from a pause, or starts a new game,
        there is a buffer time before the environment updates in order for
        the player to get ready.

        Unit: seconds
    */
    float m_fResumeTime;
    float m_fGameOverTime;
    // Signifies of the game has ended
    bool startedGameOver;

    // If the game is paused, the environment will not update
    bool m_bQueueResume;
    /*
        After the game has been signaled to end, there is some delay before the
        game actually ends. In other words, the game remains in the GameMenu
        for this duration before moving onto the PostgameMenu.

        Unit: seconds
    */
    float m_fQueueResumeTime;
    bool m_bInGame;
    /*
        If the game is paused, then the EntityManager and AIManager do not
        update. The user can be in game and yet paused at the same time. This
        would occur during the buffer time when resuming or starting a new game
        to get the player raed.
    */
    bool m_bPaused;

    vector<vec3> m_vPlayerColors;
    vector<vec3> m_vBotColors;
};
