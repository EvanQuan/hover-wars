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
    static GameManager* getInstance(GLFWwindow *rWindow, int iWindowWidth, int iWindowHeight);
    static GameManager* getInstance();
    ~GameManager();
    
    // Graphics Application
    bool initializeGraphics( string sFileName );
    bool renderGraphics();

    // HxW Settings
    void resizedWindow(int iHeight, int iWidth);

    // Camera Functions 
    //void snapShotCamera();
    void rotateCamera(vec2 pDelta);
    void zoomCamera(float fDelta);
    void intersectPlane(float fX, float fY);
    void toggleDebugCamera();

    // Must be set after getInstance() to avoid mutual getInstance() recursion
    // with CommandHandler
    CommandHandler* m_commandHandler;

    ePlayer m_eKeyboardPlayer;
#ifndef NDEBUG
    // User interface is public for debug puprposes
    // so that debug commands can change the UI directly.
    UserInterface* m_pUserInterface;
#endif

private:
    // For Singleton Implementation
    GameManager(GLFWwindow* rWindow, int iWindowWidth, int iWindowHeight); 
    GameManager(const GameManager* pCopy);
    static GameManager* m_pInstance;

    // Window Reference
    GLFWwindow* m_pWindow;
    Time m_pTimer;
    int iWindowWidth;
    int iWindowHeight;

    // Update Variables
    duration<float> m_fFrameTime;
    duration<float> m_fMaxDeltaTime;

    // Camera Boolean
    bool m_bUseDebugCamera;

    // Manager Pointers
    EntityManager* m_pEntityManager;
    ShaderManager* m_pShaderManager;

#ifdef NDEBUG
    // User interface is normally private.
    // There is no reason for it to be public.
    UserInterface* m_pUserInterface;
#endif
};

