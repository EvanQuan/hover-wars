#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"

/* CLASS */
/*

Receives user command initiates appropriate actions that correspond to that command.
Usually these commands would come from InputHandler (user input), but may come
from other means for test purposes.

*/
class CommandHandler
{
public:
	static CommandHandler* getInstance(GLFWwindow *rWindow);
	~CommandHandler();

	enum FixedCommand
	{
		ABILITY_ROCKET,
		ABILITY_SPIKES,
		ABILITY_TRAIL,
		CAMERA_CHANGE,
		DASH_BACK,
		DASH_FORWARD,
		DASH_LEFT,
		DASH_RIGHT,
		DEBUG_TOGGLE_WIREFRAME,
		MENU_BACK,
		MENU_PAUSE,
		MENU_START,
		INVALID_FIXED,
	};

	enum VariableCommand
	{
		MOVE,
		TURN,
		INVALID_VARIABLE,
	};

	void execute(int joystickID, FixedCommand command);
	void execute(int joystickID, VariableCommand command, const float x, const float y);
	/*
	Execute all commands for a given frame.
	*/
	void executeAllCommands();
	/*
	Execute all commands from user input (keyboard, mouse, controllers).
	*/
	void executeInputCommands();

	void debugToggleWireframe();

	/*
	Convert a key to its corresponding FixedCommand
	*/
	static FixedCommand keyToFixedCommand(int key)
	{
		try
		{
			return m_pInstance->m_keyToFixedCommand.at(key);
		}
		catch (...)
		{
			return INVALID_FIXED;
		}
	};
	/*
	Convert a key to its corresponding VariableCommand
	*/
	static VariableCommand keyToVariableCommand(int key)
	{
		try
		{
			return m_pInstance->m_keyToVariableCommand.at(key);
		}
		catch (...)
		{
			return INVALID_VARIABLE;
		}
	};
	/*
	Convert a joystick button to its corresponding FixedCommand
	*/
	static FixedCommand buttonToFixedCommand(int button)
	{
		try
		{
			return m_pInstance->m_buttonToFixedCommand.at(button);
		}
		catch (...)
		{
			return INVALID_FIXED;
		}
	};
	/*
	Convert a joystick axis to its corresponding FixedCommand
	*/
	static FixedCommand axisToFixedCommand(int axis)
	{
		try
		{
			return m_pInstance->m_axisToFixedCommand.at(axis);
		}
		catch (...)
		{
			return INVALID_FIXED;
		}
	};


	std::map<int, FixedCommand> m_keyToFixedCommand =
	{
		{GLFW_KEY_SPACE,       ABILITY_ROCKET},
		{GLFW_KEY_APOSTROPHE,  ABILITY_SPIKES},
		{GLFW_KEY_LEFT_SHIFT,  ABILITY_TRAIL},
		{GLFW_KEY_RIGHT_SHIFT, CAMERA_CHANGE},
		{GLFW_KEY_K,           DASH_BACK},
		{GLFW_KEY_I,           DASH_FORWARD},
		{GLFW_KEY_H,           DASH_LEFT},
		{GLFW_KEY_SEMICOLON,   DASH_RIGHT},
		{GLFW_KEY_F,           DEBUG_TOGGLE_WIREFRAME},
		{GLFW_KEY_TAB,         MENU_BACK},
		{GLFW_KEY_P,           MENU_PAUSE},
		{GLFW_KEY_ENTER,       MENU_START},
	};

	std::map<int, VariableCommand> m_keyToVariableCommand =
	{
		{GLFW_KEY_W, MOVE},
		{GLFW_KEY_A, MOVE},
		{GLFW_KEY_S, MOVE},
		{GLFW_KEY_D, MOVE},
		{GLFW_KEY_J, TURN},
		{GLFW_KEY_L, TURN},
	};

	std::map<int, FixedCommand> m_buttonToFixedCommand =
	{
		{BUTTON_LEFT_BUMPER,  ABILITY_SPIKES},
		{BUTTON_RIGHT_BUMPER, CAMERA_CHANGE},
		{BUTTON_A,            DASH_BACK},
		{BUTTON_Y,            DASH_FORWARD},
		{BUTTON_X,            DASH_LEFT},
		{BUTTON_B,            DASH_RIGHT},
		{BUTTON_START,        DEBUG_TOGGLE_WIREFRAME},
		{BUTTON_BACK,         MENU_BACK},
		{BUTTON_BACK,         MENU_PAUSE},
		{BUTTON_START,        MENU_START},
		{BUTTON_LEFT_STICK,   INVALID_FIXED},
		{BUTTON_RIGHT_STICK,  INVALID_FIXED},
		{BUTTON_UP,           INVALID_FIXED},
		{BUTTON_RIGHT,        INVALID_FIXED},
		{BUTTON_DOWN,         INVALID_FIXED},
		{BUTTON_LEFT,         INVALID_FIXED},
		{BUTTON_UNKNOWN1,     INVALID_FIXED},
		{BUTTON_UNKNOWN2,     INVALID_FIXED},
	};

	std::map<int, FixedCommand> m_axisToFixedCommand =
	{
		{AXIS_LEFT_TRIGGER,  ABILITY_TRAIL},
		{AXIS_RIGHT_TRIGGER, ABILITY_ROCKET},
	};


private:
	// Singleton Variables
	CommandHandler(GLFWwindow *rWindow);
	static CommandHandler* m_pInstance;
	EntityManager *m_pEntityManager;
	GameManager *m_pGameManager;
	InputHandler *m_pInputHandler;

	// Internal variables
	// For keyboard command handling
	FixedCommand m_pFixedCommand;
	VariableCommand m_pVariableCommand;
	float x;
	float y;


	// For input debugging
	std::map<FixedCommand, const char*> m_pFixedCommandToString =
	{
		{ABILITY_ROCKET,         "Rocket"},
		{ABILITY_SPIKES,         "Spikes"},
		{ABILITY_TRAIL,          "Trail"},
		{CAMERA_CHANGE,          "Change Camera"},
		{DASH_BACK,              "Dash back"},
		{DASH_FORWARD,           "Dash Forward"},
		{DASH_LEFT,              "Dash Left"},
		{DASH_RIGHT,             "Dash Right"},
		{DEBUG_TOGGLE_WIREFRAME, "Debug Toggle Wireframe"},
		{MENU_BACK,              "Menu Back"},
		{MENU_PAUSE,             "Menu Pause"},
		{MENU_START,             "Menu Start"},
		{INVALID_FIXED,          "Invalid"},
	};

	std::map<VariableCommand, const char*> m_pVariableCommandToString =
	{
		{MOVE,             "Move"},
		{TURN,             "Turn"},
		{INVALID_VARIABLE, "Invalid"},
	};

	void executeKeyboardCommands();
	void executeJoystickCommands();



};

