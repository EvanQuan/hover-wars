/*

Receives user input (mouse, keyboard and controller) and initiates appropriate
actions that correspond to input to CommandHandler.

*/
#include "stdafx.h"
#include "InputHandler.h"
#include "Mouse_Handler.h"

// Single Singleton instance
InputHandler* InputHandler::m_pInstance = nullptr;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMovecallback(GLFWwindow* window, double x, double y);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

InputHandler::InputHandler(GLFWwindow *rWindow)
{
	// Initializing Base Class
	m_pCommandHandler = CommandHandler::getInstance(rWindow);
	m_pMouseHandler = Mouse_Handler::getInstance(rWindow);
	glfwSetKeyCallback(rWindow, KeyCallback);
	glfwSetMouseButtonCallback(rWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(rWindow, mouseMovecallback);
	glfwSetScrollCallback(rWindow, mouseScrollCallback);
}

InputHandler* InputHandler::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new InputHandler(rWindow);
	}

	return m_pInstance;
}



InputHandler::~InputHandler()
{
	m_pCommandHandler = nullptr;
	m_pMouseHandler = nullptr;
}


// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GameManager* pGPXMngr = GameManager::getInstance(window);
	ShaderManager* pShdrMngr = SHADER_MANAGER;
	EntityManager* pEnvMngr = ENTITY_MANAGER;
	InputHandler* pInputHandler = InputHandler::getInstance(window);

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)											// Exit
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else
	{
		pInputHandler->handleKeyBoardInput(key, action, mods);
	}
}

void InputHandler::handleKeyBoardInput(int cKey, int iAction, int iMods)
{
	vec3 pMoveVec(0.f, 0.f, 0.f);
	CommandHandler::Command command = CommandHandler::NOTHING;
	switch (cKey)
	{
	case GLFW_KEY_W:
		command = CommandHandler::MOVE_FORWARD;
		break;
	case GLFW_KEY_S:
		command = CommandHandler::MOVE_BACK;
		break;
	case GLFW_KEY_A:
		command = CommandHandler::MOVE_LEFT;
		break;
	case GLFW_KEY_D:
		command = CommandHandler::MOVE_RIGHT;
		break;
	case GLFW_KEY_J:
		command = CommandHandler::TURN_LEFT;
		break;
	case GLFW_KEY_L:
		command = CommandHandler::TURN_RIGHT;
		break;
	case GLFW_KEY_K:
		command = CommandHandler::DASH_BACK;
		break;
	case GLFW_KEY_I:
		command = CommandHandler::DASH_FORWARD;
		break;
	case GLFW_KEY_H:
		command = CommandHandler::DASH_LEFT;
		break;
	case GLFW_KEY_SEMICOLON:
		command = CommandHandler::DASH_RIGHT;
		break;
	case GLFW_KEY_SPACE:
		command = CommandHandler::ABILITY_ROCKET;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		command = CommandHandler::ABILITY_TRAIL;
		break;
	case GLFW_KEY_APOSTROPHE:
		command = CommandHandler::ABILITY_SPIKES;
		break;
	case GLFW_KEY_F:
		if (GLFW_PRESS == iAction)
		{
			command = CommandHandler::DEBUG_TOGGLE_WIREFRAME;
		}
		break;
	case GLFW_KEY_ENTER:
		command = CommandHandler::MENU_SELECT;
		break;
	case GLFW_KEY_P:
		command = CommandHandler::MENU_PAUSE;
		// if ( iAction == GLFW_RELEASE )
			// m_pEntMngr->pause();
		break;
	}
	m_pCommandHandler->executeCommand(CommandHandler::PLAYER_ONE, command);
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
		{
			mMouseHndlr->mouseTStart();
		}
		else if (GLFW_RELEASE == action)
		{
			mMouseHndlr->mouseTEnd();
		}
	}
	if (GLFW_MOUSE_BUTTON_2 == button)
	{
		glfwGetCursorPos(window, &fX, &fY);
		if (GLFW_PRESS == action)
		{
			mMouseHndlr->mouseRStart();
		}
		else if (GLFW_RELEASE == action)
		{
			mMouseHndlr->mouseREnd();
		}
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
