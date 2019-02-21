/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "CommandHandler.h"
#include "SceneLoader.h"
#include "ShaderManager.h"
#include "Physics/PhysicsManager.h"

#include "..\TestClass.h"

// Function Prototypes
void ErrorCallback(int error, const char* description);
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight);
bool initializeWindow(GLFWwindow** rWindow, int* iHeight, int* iWidth, const char* cTitle);

// Main entry point for the Graphics System
int main()
{
    // Local Variables
    int iRunning = glfwInit();
    int iWindowHeight, iWindowWidth;
    GLFWwindow* m_window = 0;
    GameManager* m_gameManager = 0;
    ShaderManager* m_shaderManager = 0;
    InputHandler* m_inputHandler = 0;
    PhysicsManager* m_pPhysicsManager = 0;

    // Initialize GL and a window
    if (!iRunning)
    {
        cout << "Error: GLFW failed to intialize, ending program." << endl;
    }
    else
    {
        // Set Error Callback and init window
        glfwSetErrorCallback( ErrorCallback );
        iRunning = initializeWindow( &m_window, &iWindowHeight, &iWindowWidth, "Hover Wars" );

        // Initialize glew
        glewExperimental = GL_TRUE;
        if ( GLEW_OK != glewInit() )
        {
            cout << "Glew didn't initialize properly.  Exiting program." << endl;
            cout << "GLEW ERROR: " << glewGetErrorString( iRunning ) << endl;
            iRunning = false;
        }
        glGetError();

        if (iRunning)
        {
            // Initialize Physics
            m_pPhysicsManager = PHYSICS_MANAGER;
            m_pPhysicsManager->initPhysics(true);

            // Bind window to Game Manager
            m_gameManager = GameManager::getInstance(m_window);

            // Initialize the InputHandler for mouse, keyboard, controllers
            m_inputHandler = InputHandler::getInstance(m_window);

            // Initialize Graphics
            iRunning = !m_gameManager->initializeGraphics( STARTING_ENV );
            m_shaderManager = SHADER_MANAGER;

            // Main loop
            while (iRunning)                
                iRunning = m_gameManager->renderGraphics(); // do Graphics Loop
        }

        // Clean up!
        if (nullptr != m_gameManager)       // Game Manager
            delete m_gameManager;

        if (nullptr != m_inputHandler)      // Input Handler
            delete m_inputHandler;

        if (nullptr != m_pPhysicsManager)   // Physics Manager
            delete m_pPhysicsManager;

        // Detroy Window
        glfwDestroyWindow(m_window);
    }

    // Terminate Window
    glfwTerminate();

    // Exit Program
    return 0;
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
    (*rWindow) = glfwCreateWindow(mode->width, mode->height, cTitle, pMonitor, nullptr);
    glfwSetInputMode(*rWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide cursor in full screen
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
        GAME_MANAGER->resizedWindow(iHeight, iWidth);
}
