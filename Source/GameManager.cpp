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

    // Fetch and update Window HxW
    m_pWindow = rWindow;
    int iWidth, iHeight;
    glfwGetWindowSize(m_pWindow, &iWidth, &iHeight);
    m_pEntityManager->updateWidthAndHeight(iWidth, iHeight);

    // Initialize Timer Variables
    m_fFrameTime = duration<float>(0.0f);
    m_fMaxDeltaTime = sixtieth_of_a_sec{ 1 };

    m_eKeyboardHovercraft = HOVERCRAFT_PLAYER_1;
}

/*
Singleton Implementations
Requires Window to initialize 

@param rWindow to intialize
*/
GameManager* GameManager::getInstance(GLFWwindow *rWindow)
{
    if (nullptr == m_pInstance)
        m_pInstance = new GameManager(rWindow);

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

    // Clean up Allocated Memory
    if (nullptr != m_pEntityManager)    // Entity Manager
        delete m_pEntityManager;

    if (nullptr != m_pShaderManager)    // Shader Manager
        delete m_pShaderManager;

    if (nullptr != m_pUserInterface)    // User Interface
        delete m_pUserInterface;

    if (nullptr != m_pCommandHandler)   // Command Handler
        delete m_pCommandHandler;
}

// Intended to be called every cycle, or when the graphics need to be updated
bool GameManager::renderGraphics()
{
    // Update Timer
    m_pTimer.updateTimeSinceLastFrame();
    m_fFrameTime += m_pTimer.getFrameTimeSinceLastFrame();
    /*
    Get the delta since the last frame and update based on that delta.

    Unit: seconds
    */

    // Execute all commands for this frame
    // These should be done before the environment updates so that the
    // environemnt can respond to the commands issued this frame.
    m_pCommandHandler->update();

    // Update Environment if the gamee is not paused
    // includes UI
    if (!paused)
    {
        m_pEntityManager->updateEnvironment(&m_pTimer);
    }

    // call function to draw our scene
    while (m_fFrameTime >= sixtieth_of_a_sec{ 1 }) // This locks the framerate to 60 fps
    {
        m_fFrameTime = seconds{ 0 };

        // Render the Scene
        glEnable(GL_DEPTH_TEST);
        m_pEntityManager->renderEnvironment();
        glDisable(GL_DEPTH_TEST);

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(m_pWindow);
    }

    // check for Window events
    glfwPollEvents();

    return !glfwWindowShouldClose(m_pWindow);
}

/*
Function initializes shaders and geometry.
contains any initializion requirements in order to start drawing.

@param sFileName    filepath to a proper .scene file that contains the
                    necessary information about shaders,  and geometry
                    in the scene.
@return true if the shaders failed to initialize.
*/
bool GameManager::initializeGraphics( string sFileName )
{
    // Locals
    bool bError = false;
    int iWidth, iHeight;

    // Shaders
    if (!m_pShaderManager->initializeShaders())
    {
        cout << "Couldn't initialize shaders." << endl;
        bError = true;
    }
    else
    {
        // TODO: This will be done once a level is chosen to load.
        // Initialize User Interface
        glfwGetWindowSize(m_pWindow, &iWidth, &iHeight);
        m_pUserInterface = UserInterface::getInstance(iWidth, iHeight);

        // Initialize Environment with a new scene
        m_pEntityManager->initializeEnvironment(sFileName);         
        m_pCommandHandler = CommandHandler::getInstance(m_pWindow); // Initialize Command Handler; Game Manager will manage and clean up this memory
    }

    // Return error results
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

/*
Called from Window Resize callback.
Currently updates the Entity Manager with new Window Size, may require more
functionality for menus, etc.

@param iWidth   of the window after resizing
@param iHeight  of the window after resizing
*/
void GameManager::resizeWindow( int iWidth, int iHeight )
{
    m_pEntityManager->updateWidthAndHeight(iWidth, iHeight);
    m_pUserInterface->updateWidthAndHeight(iWidth, iHeight);
}

/*
Calculates an intersection given screen coordinates.
This is used for testing the particle emitter by spawning an emitter where
the mouse clicks the floor plane.

@param fX   x-coordinate of the mouse in window coordinates
@param fY   y-coordinate of the mouse in window coordinates
*/
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
