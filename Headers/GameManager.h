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
class UserInterface;
class GameStats;
class Texture;

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
    void initializeNewGame(unsigned int playerCount, unsigned int botCount, float gameTime, string sFileName);
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
    void setCurrentInterface(UserInterface* ui);

    // Automatically called at end of game if time runs out, or if ended thrugh
    // pause menu
    void endGame();

    void flagWindowToClose() { glfwSetWindowShouldClose(m_pWindow, GL_TRUE); }

private:
    // For Singleton Implementation
    GameManager(GLFWwindow* rWindow); 
    GameManager(const GameManager* pCopy);
    static GameManager* m_pInstance;

    bool renderGraphics();
    void drawScene();
    void renderSplitScreen();

    // Split Screen Rendering variables
    GLuint m_iVertexArray, m_iVertexBuffer;
    struct sRenderBlock
    {
        GLuint iRenderBuffer, iFrameBuffer;
        Texture* pColorBuffer;
    };
    vector< sRenderBlock > m_pFrameBufferTextures;
    void generateSplitScreen(unsigned int iPlayer);
    void cleanupFrameBuffers();
    void generateFrameBuffer(unsigned int iPlayer);

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
    This is tracked to ensure that all values update correctly for every frame
    update.

    Unit: seconds
    */
    duration<float> m_fFrameTime;
    /*
    This depends the rate at which the window is re-rendered.
    Since it is costly to re-render the window, it is locked to re-render at a
    fixed rate, even if the enviroment updates at a faster rate.

    Unit: seconds
    */
    duration<float> m_fMaxDeltaTime;

    /*
    After the game has ended, there is a duration of slow motion before the
    game moves to the post game menu.

    Unit: seconds
    */
    float m_fGameTime;
    /*
    After the game has been signaled to end, there is some delay before the
    game actually ends. In other words, the game remains in the GameMenu for
    this duration before moving onto the PostgameMenu.

    Unit: seconds
    */
    float m_fGameOverTime;
    // Signifies of the game has ended
    bool startedGameOver;

    // Manager Pointers
    EntityManager*      m_pEntityManager;
    ShaderManager*      m_pShaderManager;
    CommandHandler*     m_pCommandHandler;
    AIManager*          m_pAIManager;
    GameTime            m_pTimer;
    GameStats*          m_pGameStats;
    PhysicsManager*     m_pPhysicsManager;
    UserInterface*      m_pCurrentInterface;
    vector<UserInterface*> m_vInterfaceInstances;

    // If the game is paused, the environment will not update
    bool m_bQueueResume;
    float m_fQueueResumeTime;
    bool m_bPaused;

    void setKeyboardHovercraft(int playerCount);
    // The keyboard corresponds to its own hovercraft
    // which might be shared with a joystick
    eHovercraft m_eKeyboardHovercraft;
};
