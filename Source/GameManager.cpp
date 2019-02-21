#include "EntityManager.h"
#include "GameManager.h"
#include "CommandHandler.h"
#include "SceneLoader.h"
#include "ShaderManager.h"
#include "UserInterface/UserInterface.h"

/*************\
 * Constants *
\*************/


// Singleton Variable initialization
GameManager* GameManager::m_pInstance = nullptr;

// Constructor - Private, only accessable within the Graphics Manager
GameManager::GameManager(GLFWwindow* rWindow)
{
    // Initialize and Get Shader and Environment Managers
    m_pShaderManager    = SHADER_MANAGER;
    m_pEntityManager    = ENTITY_MANAGER;
    m_pUserInterface = UserInterface::getInstance(rWindow);

    m_pUserInterface->setDisplayCount(0);

    m_pWindow = rWindow;
    int iHeight, iWidth;
    glfwGetWindowSize(m_pWindow, &iWidth, &iHeight);

    m_fFrameTime = duration<float>(0.0f);
    m_fMaxDeltaTime = sixtieths_of_a_sec{ 1 };

    m_bUseDebugCamera = false;

    m_eKeyboardPlayer = PLAYER_1;
}

// Singleton Implementations
// Requires Window to initialize 
GameManager* GameManager::getInstance(GLFWwindow *rWindow)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new GameManager(rWindow);
    }

    return m_pInstance;
}

// Does not require window parameter, but assumes that Singleton instance has
// been instantiated beforehand.
GameManager* GameManager::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

// Destruct Shaders, Buffers, Arrays and other GL stuff.
GameManager::~GameManager()
{
    // Let go of Window Handle
    m_pWindow = nullptr;

    // Let go of Manager Handles
    if (nullptr != m_pEntityManager)
    {
        delete m_pEntityManager;
    }

    if (nullptr != m_pShaderManager)
    {
        delete m_pShaderManager;
    }

    if (nullptr != m_pUserInterface)
    {
        delete m_pUserInterface;
    }
}

// Intended to be called every cycle, or when the graphics need to be updated
bool GameManager::renderGraphics()
{
    // Update Timer
    m_pTimer.updateTime();
    m_fFrameTime += m_pTimer.getFrameTime();

    // Execute all commands for this frame
    m_commandHandler->executeAllCommands();

    // Update Environment
    m_pEntityManager->updateEnvironment(m_pTimer);

    // call function to draw our scene
    if (m_fFrameTime >= m_fMaxDeltaTime) // This locks the framerate to 60 fps
    {
        m_fFrameTime = duration<float>(0.0f);

        glEnable(GL_DEPTH_TEST);

        m_pEntityManager->renderEnvironment();

        glDisable(GL_DEPTH_TEST);

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(m_pWindow);
    }

    // TODO the user interface updating may need to change to account for
    // time, similar to how the EntityManager does it.
    m_pUserInterface->update();

    // check for Window events
    glfwPollEvents();

    return !glfwWindowShouldClose(m_pWindow);
}

// Function initializes shaders and geometry.
// contains any initializion requirements in order to start drawing.
bool GameManager::initializeGraphics( string sFileName )
{
    // Locals
    bool bError = false;

    // Shaders
    if (!m_pShaderManager->initializeShaders())
    {
        cout << "Couldn't initialize shaders." << endl;
        bError = true;
    }
    else
    {
        m_pEntityManager->initializeEnvironment(sFileName);
    }

    m_pTimer.resetTimer();
    return bError; 
}

/*******************************************************************************\
 * Camera Manipulation                                                         *
\*******************************************************************************/

void GameManager::rotateCamera(vec2 pDelta)
{
    m_pEntityManager->rotateCamera(pDelta);
}

void GameManager::zoomCamera(float fDelta)
{
    m_pEntityManager->zoomCamera(fDelta);
}

void GameManager::resizedWindow( int iHeight, int iWidth )
{
    m_pEntityManager->updateHxW(iHeight, iWidth);
}

void GameManager::toggleDebugCamera()
{
    m_bUseDebugCamera = !m_bUseDebugCamera;
    m_pEntityManager->toggleDebugCamera();
    if (m_bUseDebugCamera)
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

// Calculates an intersection given screen coordinates.
// This is used for testing the particle emitter by spawning an emitter where
// the mouse clicks the floor plane.
void GameManager::intersectPlane(float fX, float fY)
{
    // Local Variables
    vec3 vRay = m_pEntityManager->getActiveCameraComponent()->getRay(fX, fY);
    vec3 vNormal = vec3(0.0, 1.0, 0.0); // normal of xz-plane
    vec3 vCameraPos = m_pEntityManager->getActiveCameraComponent()->getCameraWorldPos();
    vec3 vIntersection = vec3(-1.0f);
    float fT = dot(vRay, vNormal);

    // Calculate Intersection
    if ((fT > FLT_EPSILON) || (fT < -FLT_EPSILON))
    {
        // Is intersecting.
        fT = -(dot(vCameraPos, vNormal) / fT);

        // Not behind camera.
        if (fT >= 0)
            vIntersection = vCameraPos + (fT*vRay);

        EMITTER_ENGINE->generateEmitter(vIntersection, vNormal, 60.f, 5.0f, 100, false, 2.0f);
    }
}
