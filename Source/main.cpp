/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "CommandHandler.h"
#include "SceneLoader.h"
#include "ShaderManager.h"
#include "Physics/PhysicsManager.h"
#include "SoundManager.h"

// This sets the window title
#define PROGRAM_NAME "Hover Wars"

// Function Prototypes
void ErrorCallback(int error, const char* description);
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight);
bool initializeWindow(GLFWwindow** rWindow, int* iHeight, int* iWidth, const char* cTitle);

// Organizational function prototypes
void initializeWindow();
void initializeGLEW();
bool initializeManagers();
void cleanup();

// These are not local variables to main so that other functions can better
// access them in main.cpp
GLFWwindow* m_window = 0;
GameManager* m_gameManager = 0;
InputHandler* m_inputHandler = 0;
SoundManager* m_soundManager = 0;
int iRunning;
int iWindowHeight, iWindowWidth;

// Main entry point for the Graphics System
int main()
{
    iRunning = glfwInit();

    // Initialize GL and a window
    if (!iRunning)
    {
        cout << "Error: GLFW failed to intialize, ending program." << endl;
    }
    else
    {
        initializeWindow();
        initializeGLEW();
        if (iRunning) // only succeeds if both glfw and glew are successful
        {
            if (initializeManagers())
            {
                m_gameManager->startRendering();
            }
        }
        cleanup();
    }
    glfwTerminate();    // Terminate Window
    return 0;           // Exit program with success
}

void initializeWindow()
{
    // Set Error Callback and init window
    glfwSetErrorCallback( ErrorCallback );
    iRunning = initializeWindow( &m_window, &iWindowHeight, &iWindowWidth, PROGRAM_NAME );
}

void initializeGLEW()
{
    // Initialize glew
    glewExperimental = GL_TRUE; // TODO unused variable?
    if ( GLEW_OK != glewInit() )
    {
        cout << "Glew didn't initialize properly.  Exiting program." << endl;
        cout << "GLEW ERROR: " << glewGetErrorString( iRunning ) << endl;
        iRunning = false;
    }
    glGetError();
}

/*
    Initialize all core values at startup.
    This should only be called once at the beginning of the program.
*/
bool initializeManagers()
{
    // Bind window to Game Manager
    m_gameManager = GameManager::getInstance(m_window);

    // Initialize the InputHandler for mouse, keyboard, controllers
    m_inputHandler = InputHandler::getInstance(m_window);

    // Initialize Sound
    m_soundManager = SOUND_MANAGER;
    m_soundManager->loadFiles();

    return m_gameManager->initialize();
}

/*
    Delete all singletons
    Can't have those pesky memory leaks
*/
void cleanup()
{
    // Clean up!
    if (nullptr != m_gameManager)       // Game Manager
        delete m_gameManager;

    if (nullptr != m_inputHandler)      // Input Handler
        delete m_inputHandler;

    if (nullptr != m_soundManager)
        delete m_soundManager;

    glfwDestroyWindow(m_window);
}

// For reporting GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW Error : " << error << ":" << endl;
    cout << description << endl;
}

/*
 * @return true if window successfully initialized
 */
bool initializeWindow(GLFWwindow** rWindow, int* iHeight, int* iWidth, const char* cTitle)
{
    GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(pMonitor);

    // Set Window Hints based on
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    // GLFWwidmode mode reveals the monitor dimensions of the user.
    //      mode->height     mode->width
    // We can use these dimensions to dynamically create a window (and thus the User Interface)
    // to match the user's monitor, instead of using hardcoded values.
    // return the Monitor's Height and Width
    *iHeight = mode->height;
    *iWidth = mode->width;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef NDEBUG
    // Full screen with cursor hidden
    //(*rWindow) = glfwCreateWindow(mode->width, mode->height, cTitle, pMonitor, nullptr);
    //glfwSetInputMode(*rWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide cursor in full screen
    (*rWindow) = glfwCreateWindow(mode->width, mode->height, cTitle, nullptr, nullptr);
#else
    (*rWindow) = glfwCreateWindow(mode->width, mode->height, cTitle, nullptr, nullptr);
#endif
    if (!*rWindow)
    {
        cout << "Program failed to create GLFW window" << endl;
        return false;
    }
    glfwSetWindowSizeCallback(*rWindow, WindowResizeCallback);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwMakeContextCurrent(*rWindow);
    return true;
}

// handles Window Resize events
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight)
{
    // Update the viewport information
    glViewport(0, 0, iWidth, iHeight);

    if ((iWidth != 0) && (iHeight != 0))
        GAME_MANAGER->resizeWindow(iWidth, iHeight);
}
