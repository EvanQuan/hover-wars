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
    bool initializeGraphics( string sFileName );
    void startRendering();
    void initializeNewGame(int playerCount, int botCount, float gameTime);
    void resetTime() { m_pTimer.resetTimer(); }

    // Window Width and Height  Settings
    void resizeWindow(int iWidth, int iHeight);

    // Camera Functions 
    //void snapShotCamera();
    void rotateCamera(vec2 pDelta);
    void zoomCamera(float fDelta);
    void intersectPlane(float fX, float fY);

    // The keyboard corresponds to its own hovercraft
    // which might be shared with a joystick
    eHovercraft m_eKeyboardHovercraft;
#ifndef NDEBUG
    // User interface is public for debug puprposes
    // so that debug commands can change the UI directly.
    // TODO: Fix this design so that it doesn't change depending on Debug or Not
    UserInterface* m_pUserInterface;
#endif

    bool isPaused() const { return paused; }
    void setPaused(bool paused) { this->paused = paused; }

private:
    // For Singleton Implementation
    GameManager(GLFWwindow* rWindow); 
    GameManager(const GameManager* pCopy);
    static GameManager* m_pInstance;

    bool renderGraphics();
    void drawScene();

    void endGame();

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

    // If the game is paused, the environment will not update
    bool paused;

#ifdef NDEBUG
    // User interface is normally private.
    // There is no reason for it to be public.
    UserInterface* m_pUserInterface;
#endif
};

