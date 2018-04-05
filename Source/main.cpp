/* INCLUDES */
#include "stdafx.h"
#include "GraphicsManager.h"
#include "ShaderManager.h"
#include "Mouse_Handler.h"
#include "Object_Factory.h"
#include "EnvironmentManager.h"
#include "CmdHandler.h"

#ifdef USING_LINUX
		#include <Magick++.h>
#endif

/* DEFINES */
#define INPUT_SIZE			128
#define START_HEIGHT		512
#define START_WIDTH			512
#define STARTING_ENV		"scene2.txt"
#define LIGHT_MOVE_FACTOR	0.05

// Function Prototypes
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight);
void mouseMovecallback(GLFWwindow* window, double x, double y);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
bool initializeWindow(GLFWwindow** rWindow, int iHeight, int iWidth, const char* cTitle);

// Main entry point for the Graphics System
int main()
{
	int iRunning = glfwInit();
	char c_Input[INPUT_SIZE] = {};
	GLFWwindow* m_Window = 0;
	GraphicsManager* m_GpxMngr = 0;
	ShaderManager* m_ShdrMngr = 0;
	Mouse_Handler* m_MseHndlr = 0;

	// Initialize GL and a window
	if (!iRunning)
		cout << "Error: GLFW failed to intialize, ending program." << endl;
	else
	{
		// Set Error Callback and init window
		glfwSetErrorCallback( ErrorCallback );
		iRunning = initializeWindow( &m_Window, START_HEIGHT, START_WIDTH, "Animation" );

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
			if (iRunning )
				m_GpxMngr = GraphicsManager::getInstance( m_Window );

			// Initialize the Mouse Handler.
			m_MseHndlr = Mouse_Handler::getInstance( m_Window );

			// Initialize Graphics
			iRunning = !m_GpxMngr->initializeGraphics( STARTING_ENV );
			m_ShdrMngr = ShaderManager::getInstance();

		#ifdef USING_WINDOWS
			m_ShdrMngr->setUniformBool( ShaderManager::eShaderType::MESH_SHDR, "bUsingLinux", false );
		#endif

			// Main loop
			while ( iRunning )
			{
				// do Graphics Loop
				iRunning = m_GpxMngr->renderGraphics();
			}
		}

		// Clean up!
		if (m_GpxMngr != nullptr)
			delete m_GpxMngr;

		if( m_MseHndlr != nullptr )
		  delete m_MseHndlr;

		glfwDestroyWindow(m_Window);
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
		glfwSetKeyCallback(*rWindow, KeyCallback);
		glfwSetWindowSizeCallback(*rWindow, WindowResizeCallback);
		glfwSetMouseButtonCallback(*rWindow, mouseButtonCallback);
		glfwSetCursorPosCallback(*rWindow, mouseMovecallback);
		glfwSetScrollCallback(*rWindow, mouseScrollCallback);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwMakeContextCurrent(*rWindow);
	}

	return bSuccess;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GraphicsManager* pGPXMngr = GraphicsManager::getInstance(window);
	ShaderManager* pShdrMngr = ShaderManager::getInstance();
	EnvironmentManager* pEnvMngr = EnvironmentManager::getInstance();
	CmdHandler* pCmdHndlr = CmdHandler::getInstance(window);

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)											// Exit
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (GLFW_KEY_ENTER == key && GLFW_PRESS == action)
	{
		// Hide Window to Prompt User
		glfwHideWindow(window);

		pCmdHndlr->process_Input();

		// Show Window for results
		glfwShowWindow(window);
	}
	else
		pCmdHndlr->handleKeyBoardInput(key, action, mods);
}

// handles Window Resize events
void WindowResizeCallback(GLFWwindow* window, int iWidth, int iHeight)
{
	// Update the viewport information
	glViewport(0, 0, iWidth, iHeight);

	GraphicsManager::getInstance(window)->resizedWindow(iHeight, iWidth);
}

// Mouse Button Callback
// Handle mouse movement controls.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Mouse_Handler* mMouseHndlr = Mouse_Handler::getInstance( window );
	double fX, fY;

	if (GLFW_MOUSE_BUTTON_1 == button)
	{
		glfwGetCursorPos(window, &fX, &fY);
		if (GLFW_PRESS == action)
			mMouseHndlr->mouseTStart();
		else if (GLFW_RELEASE == action)
			mMouseHndlr->mouseTEnd();
	}
	if (GLFW_MOUSE_BUTTON_2 == button)
	{
		glfwGetCursorPos(window, &fX, &fY);
		if (GLFW_PRESS == action)
			mMouseHndlr->mouseRStart();
		else if (GLFW_RELEASE == action)
			mMouseHndlr->mouseREnd();
	}
}

// Handles input from Mouse Moves.
void mouseMovecallback(GLFWwindow* window, double x, double y)
{
	Mouse_Handler* mMouseHndlr = Mouse_Handler::getInstance(window);

	mMouseHndlr->updateMouse((float)x, (float)y);
}

// Handle scroll wheel callbacks
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Mouse_Handler* pMsHndlr = Mouse_Handler::getInstance(window);

	pMsHndlr->mouseZoom((float)yoffset * 0.05f);
}
