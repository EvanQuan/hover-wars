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

    /// HxW Settings
    void resizedWindow(int iHeight, int iWidth);

    // Camera Functions 
    //void snapShotCamera();
    void rotateCamera(vec2 pDelta);
    void zoomCamera(float fDelta);
    void moveCamera( vec3 pDirection );
    void intersectPlane(float fX, float fY);
    void switchView();
    void toggleDebugCamera()
    {
        m_bUseDebugCamera = !m_bUseDebugCamera;
        if (m_bUseDebugCamera)
            glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else 
            glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    // Helper Enum for RGB Values
    enum eRGB
    {
        RED = 0,
        GREEN,
        BLUE,
        RGB_MAX
    };

    // Must be set after getInstance() to avoid mutual getInstance() recursion
    // with CommandHandler
    CommandHandler* m_commandHandler;

private:
    // For Singleton Implementation
    GameManager(GLFWwindow* rWindow); 
    GameManager(const GameManager* pCopy);
    static GameManager* m_pInstance;

    // Window Reference
    GLFWwindow* m_pWindow;
    Time m_pTimer;

    // Axis Buffer/Array Containers
    GLuint m_pVertexArray;
    GLuint m_pVertexBuffer;

    // Update Variables
    duration<float> m_fFrameTime;
    duration<float> m_fMaxDeltaTime;

    // Camera
    bool m_bUseDebugCamera;
    Camera* m_pCamera;
    enum cView
    {
        VIEW_SPHERICAL = 0,
        VIEW_FPS,
        VIEW_FOLLOW,
        VIEW_MAX
    } m_eView;

    // Render Functions
    void RenderScene();
    void renderAxis();

    // Manager Pointers
    EntityManager* m_pEntityManager;
    ShaderManager* m_pShaderManager;
    UserInterface* m_pUserInterface;

};

