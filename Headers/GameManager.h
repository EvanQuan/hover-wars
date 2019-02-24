#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityHeaders/Camera.h"
#include <time.h>

// Forward Declarations
class EntityManager;
class CommandHandler;
class ShaderManager;
class UserInterface;

// Class: Graphics Manager
// Purpose: Acts as the Sinew between all moving parts that are required for drawing
//          with openGL.
// TODO: Set-up a Manager for Geometry (Geometry may be expanded on later).
// Written by: James Coté
class GameManager
{
public:
    static GameManager* getInstance(GLFWwindow *rWindow);
    static GameManager* getInstance();
    ~GameManager();
    
    // Graphics Application
    bool initializeGraphics( string sFileName );
    bool renderGraphics();
    void resetTime() { m_pTimer.resetTimer(); }

    // Window Width and Height  Settings
    void resizeWindow(int iWidth, int iHeight);

    // Camera Functions 
    //void snapShotCamera();
    void rotateCamera(vec2 pDelta);
    void zoomCamera(float fDelta);
    void intersectPlane(float fX, float fY);

    ePlayer m_eKeyboardPlayer;
#ifndef NDEBUG
    // User interface is public for debug puprposes
    // so that debug commands can change the UI directly.
    // TODO: Fix this design so that it doesn't change depending on Debug or Not
    UserInterface* m_pUserInterface;
#endif

private:
    // For Singleton Implementation
    GameManager(GLFWwindow* rWindow); 
    GameManager(const GameManager* pCopy);
    static GameManager* m_pInstance;

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

    // Manager Pointers
    EntityManager* m_pEntityManager;
    ShaderManager* m_pShaderManager;
    CommandHandler* m_pCommandHandler;
    GameTime m_pTimer;

#ifdef NDEBUG
    // User interface is normally private.
    // There is no reason for it to be public.
    UserInterface* m_pUserInterface;
#endif
};

