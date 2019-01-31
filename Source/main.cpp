/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "CommandHandler.h"
#include "Scene_Loader.h"
#include "ShaderManager.h"

#ifdef USING_LINUX
		#include <Magick++.h>
#endif

// Function Prototypes
void ErrorCallback(int error, const char* description);
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight);
bool initializeWindow(GLFWwindow** rWindow, int iHeight, int iWidth, const char* cTitle);

// Main entry point for the Graphics System
int main()
{
	int iRunning = glfwInit();
	char c_Input[INPUT_SIZE] = {};
	GLFWwindow* m_window = 0;
	GameManager* m_gameManager = 0;
	ShaderManager* m_shaderManager = 0;
	InputHandler* m_inputHandler = 0;


	// Initialize GL and a window
	if (!iRunning)
	{
		cout << "Error: GLFW failed to intialize, ending program." << endl;
	}
	else
	{
		// Set Error Callback and init window
		glfwSetErrorCallback( ErrorCallback );
		iRunning = initializeWindow( &m_window, START_HEIGHT, START_WIDTH, "Animation" );

		#ifdef USING_LINUX
				Magick::InitializeMagick("");	// Initializing Magick for Linux Only.
		#endif

		#ifdef USING_WINDOWS
				// Initialize glew
				glewExperimental = GL_TRUE;
				if ( GLEW_OK != glewInit() )
				{
					cout << "Glew didn't initialize properly.  Exiting program." << endl;
					cout << "GLEW ERROR: " << glewGetErrorString( iRunning ) << endl;
					iRunning = false;
				}
				glGetError();
		#endif

		if ( iRunning )
		{
			
			// Bind window to graphics Manager
			if (iRunning)
			{
				m_gameManager = GameManager::getInstance(m_window);
				m_gameManager->m_commandHandler = CommandHandler::getInstance(m_window);
			}

			// Initialize the InputHandler for mouse, keyboard, controllers
			m_inputHandler = InputHandler::getInstance(m_window);
			m_inputHandler->m_gameManager = GameManager::getInstance(m_window);

			// Initialize Graphics
			iRunning = !m_gameManager->initializeGraphics( STARTING_ENV );
			m_shaderManager = SHADER_MANAGER;

		#ifdef USING_WINDOWS
			m_shaderManager->setUniformBool( ShaderManager::eShaderType::TOON_SHDR, "bUsingLinux", false );
		#endif

			// Main loop
			while ( iRunning )
			{
				// do Graphics Loop
				iRunning = m_gameManager->renderGraphics();
			}
		}

		// Clean up!
		if (nullptr != m_gameManager)
		{
			delete m_gameManager;
		}

		if (nullptr != m_inputHandler)
		{
			delete m_inputHandler;
		}

		glfwDestroyWindow(m_window);
	}

	glfwTerminate();

	cout << "Finished Program, au revoir!" << endl;

#ifdef DEBUG // To Read any debug info output in console before ending.
	cin.get();
#endif

	return 0;
}

// For reporting GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW Error : " << error << ":" << endl;
	cout << description << endl;
}

bool initializeWindow(GLFWwindow** rWindow, int iHeight, int iWidth, const char* cTitle)
{
	bool bSuccess = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	(*rWindow) = glfwCreateWindow(iWidth, iHeight, cTitle, nullptr, nullptr);

	if (!*rWindow)
	{
		cout << "Program failed to create GLFW window" << endl;
		bSuccess = false;
	}
	else
	{
		glfwSetWindowSizeCallback(*rWindow, WindowResizeCallback);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwMakeContextCurrent(*rWindow);
	}

	return bSuccess;
}
// handles Window Resize events
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight)
{
	// Update the viewport information
	glViewport(0, 0, iWidth, iHeight);

	GameManager::getInstance(window)->resizedWindow(iHeight, iWidth);
}