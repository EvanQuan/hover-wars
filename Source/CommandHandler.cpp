/*

Receives user command initiates appropriate actions that correspond to that command.
Usually these commands would come from InputHandler (user input), but may come
from other means for test purposes.

*/
#include <iostream>
#include "CommandHandler.h"
#include "Scene_Loader.h"

/**********\
* DEFINES *
\**********/
#define INPUT_SIZE 128

CommandHandler* CommandHandler::m_pInstance = nullptr;

CommandHandler::CommandHandler(GLFWwindow *rWindow)
{
	// Initializing Base Class
	// NOTE: Do not get an instance of GameManager here or there will be
	// infinite mutual recursion and a call stack overflow
	// m_pGPXMngr = GameManager::getInstance(rWindow);
	// m_pEntMngr = ENTITY_MANAGER;

	bWireFrame = false;
}

// Get a copy of CmdHandler that doesn't have any initialized
//   Input associated with it.
CommandHandler* CommandHandler::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new CommandHandler(rWindow);
	}

	return m_pInstance;
}

// Destructor
CommandHandler::~CommandHandler()
{
	// TODO delete singleton instance?
	// m_pEntMngr = nullptr;
}

void CommandHandler::executeCommand(PlayerNumber player, Command command)
{
	// vec3 pMoveVec(0.f, 0.f, 0.f);
	switch (command)
	{
	case ABILITY_ROCKET:			// Move Light Forward (along Z)
	case ABILITY_SPIKES:
	case ABILITY_TRAIL:
	case CAMERA_FLIP:
	case DASH_BACK:
	case DASH_FORWARD:
	case DASH_LEFT:
	case DASH_RIGHT:
	case MENU_PAUSE:
	case MENU_SELECT:
	case MENU_START:
	case MOVE_BACK:
	case MOVE_FORWARD:
	case MOVE_LEFT:
	case MOVE_RIGHT:
	case TURN_LEFT:
	case TURN_RIGHT:
		std::cout << m_pPlayerNumberToString.at(player) << ": "
		          << m_pCommandToString.at(command) << std::endl;
		// std::cout << "hi" << std::endl;
		break;
	case DEBUG_TOGGLE_WIREFRAME:
		bWireFrame = !bWireFrame;
		if (bWireFrame)
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		else
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		break;
	// case GLFW_KEY_ENTER:		// Input Handling (TODO)
		// break;
	// case GLFW_KEY_UP:
		// break;
	// case GLFW_KEY_C:
		// m_pGPXMngr->switchView();
		// break;
	// case GLFW_KEY_P:
		// m_pEntMngr->pause();
		// break;
	}
}
