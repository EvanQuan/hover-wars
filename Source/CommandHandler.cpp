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
	// m_pGameManager = GameManager::getInstance();
	// m_pEntityManger = ENTITY_MANAGER;
	m_pInputHandler = InputHandler::getInstance(rWindow);
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
	// m_pEntMngr = nullptr;
	// NOTE: These are deleted in main
	m_pInputHandler = nullptr;
	m_pGameManager = nullptr;
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

void CommandHandler::executeAllCommands()
{
	executeInputCommands();
}

void CommandHandler::executeInputCommands()
{
	system("CLS"); // Clear the terminal
	executeKeyboardCommands();
	executeJoystickCommands();
}

void CommandHandler::executeKeyboardCommands()
{
	for (int key = 0; key < KEYS; key++)
	{
		if (m_pInputHandler->pressed[key])
		{
			// Fixed
			switch (key)
			{
			case GLFW_KEY_F:
				if (m_pInputHandler->pressed[key] == GLFW_PRESS)
				{
					m_pFixedCommand = DASH_BACK;
				}
				else
				{
					m_pFixedCommand = INVALID_FIXED;
				}
				break;
			case GLFW_KEY_K:
				m_pFixedCommand = DASH_BACK;
				break;
			case GLFW_KEY_I:
				m_pFixedCommand = DASH_FORWARD;
				break;
			case GLFW_KEY_H:
				m_pFixedCommand = DASH_LEFT;
				break;
			case GLFW_KEY_SEMICOLON:
				m_pFixedCommand = DASH_RIGHT;
				break;
			case GLFW_KEY_SPACE:
				m_pFixedCommand = ABILITY_ROCKET;
				break;
			case GLFW_KEY_LEFT_SHIFT:
				m_pFixedCommand = ABILITY_TRAIL;
				break;
			case GLFW_KEY_APOSTROPHE:
				m_pFixedCommand = ABILITY_SPIKES;
				break;
			case GLFW_KEY_TAB:
				m_pFixedCommand = MENU_BACK;
				break;
			case GLFW_KEY_ENTER:
				m_pFixedCommand = MENU_START;
				break;
			case GLFW_KEY_P:
				m_pFixedCommand = MENU_PAUSE;
				// if ( iAction == GLFW_RELEASE )
					// m_pEntMngr->pause();
				break;
			default:
				m_pFixedCommand = INVALID_FIXED;
			}
			if (INVALID_FIXED != m_pFixedCommand) {
				execute(m_pInputHandler->m_keyboardPlayer, m_pFixedCommand);
			}
			else
			{
				// Variable commands
				switch (key)
				{
				case GLFW_KEY_W:
					m_pVariableCommand = MOVE;
					x = JOYSTICK_MAX;
					y = JOYSTICK_NEUTRAL;
					break;
				case GLFW_KEY_S:
					m_pVariableCommand = MOVE;
					x = JOYSTICK_MIN;
					y = JOYSTICK_NEUTRAL;
					break;
				case GLFW_KEY_A:
					m_pVariableCommand = MOVE;
					x = JOYSTICK_NEUTRAL;
					y = JOYSTICK_MIN;
					break;
				case GLFW_KEY_D:
					m_pVariableCommand = MOVE;
					x = JOYSTICK_NEUTRAL;
					y = JOYSTICK_MAX;
					break;
				case GLFW_KEY_J:
					m_pVariableCommand = TURN;
					x = JOYSTICK_MIN;
					y = JOYSTICK_NEUTRAL;
					break;
				case GLFW_KEY_L:
					m_pVariableCommand = TURN;
					x = JOYSTICK_MAX;
					y = JOYSTICK_NEUTRAL;
					break;
				default:
					m_pVariableCommand = INVALID_VARIABLE;
				}
				if (INVALID_VARIABLE != m_pVariableCommand)
				{
					execute(m_pInputHandler->m_keyboardPlayer, m_pVariableCommand, x, y);
				}
			}
		}
	}
}

void CommandHandler::executeJoystickCommands()
{
	m_pInputHandler->updateJoysticks();

	for (int joystickID = GLFW_JOYSTICK_1; joystickID < MAX_PLAYER_COUNT; joystickID++)
	{
		const float* axes = m_pInputHandler->m_pJoystickAxes[joystickID];
		const unsigned char* buttonsPressed = m_pInputHandler->m_pJoystickButtonsPressed[joystickID];
		int joystickIsPresent = m_pInputHandler->m_pJoystickIsPresent[joystickID];
		if (joystickIsPresent)
		{

			// Check buttons
			for (int button = BUTTON_A; button < BUTTON_LEFT; button++)
			{
				if (buttonsPressed[button])
				{
					execute(joystickID, buttonToFixedCommand(button));
				}
			}

			// Check axes
			// Joystick axes will not be remappable, so no need to make code generalizable
			execute(joystickID, MOVE, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
			execute(joystickID, TURN, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);

			// NOTE: With works with the assumption that triggers are mapped to fixed commands
			// If we decide that triggers work better for variable commands, then we will need to change this.
			if (axes[AXIS_LEFT_TRIGGER] > TRIGGER_NETURAL)
			{
				execute(joystickID, axisToFixedCommand(AXIS_LEFT_TRIGGER));
			}
			if (axes[AXIS_RIGHT_TRIGGER] > TRIGGER_NETURAL)
			{
				execute(joystickID, axisToFixedCommand(AXIS_RIGHT_TRIGGER));
			}
		}
	}
}
