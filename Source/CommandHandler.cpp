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

void CommandHandler::execute(int joystickID, FixedCommand command)
{
	switch (command)
	{
	case ABILITY_ROCKET:
	case ABILITY_SPIKES:
	case ABILITY_TRAIL:
	case CAMERA_CHANGE:
	case DASH_BACK:
	case DASH_FORWARD:
	case DASH_LEFT:
	case DASH_RIGHT:
	case MENU_BACK:
	case MENU_PAUSE:
	case MENU_START:
		std::cout << "Player " << joystickID << ": "
		          << m_pFixedCommandToString.at(command) << std::endl;
		break;
	case DEBUG_TOGGLE_WIREFRAME:
		bWireFrame = !bWireFrame;
		if (bWireFrame)
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		else
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		std::cout << "Player " << joystickID << ": "
		          << m_pFixedCommandToString.at(command) << std::endl;
	}
}

void CommandHandler::execute(int joystickID, VariableCommand command, const float x, const float y)
{
	switch (command)
	{
	case MOVE:
	case TURN:
		std::cout << "Player " << joystickID << ": "
		          << m_pVariableCommandToString.at(command) << std::endl
		          << "\tx: " << x << std::endl
		          << "\ty: " << y << std::endl;
		break;
	}
}
