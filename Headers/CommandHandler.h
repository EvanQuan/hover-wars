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

	void execute(ePlayer player, FixedCommand command);
	void execute(ePlayer player, eVariableCommand command, float x, float y);
	/*
	Execute all commands for a given frame.
	*/
	void executeAllCommands();
	/*
	Execute all commands from user input (keyboard, mouse, controllers).
	*/
	void executeInputCommands();

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
	eVariableCommand m_pVariableCommand;
	float xMove;
	float yMove;
	float xTurn;
	float yTurn;

	void normalize(float& x, float& y);
	float getMagnitude(float x, float y);
	bool magnitudeIsNeutral(float magnitude);

	void executeKeyboardCommands();
	void executeJoystickCommands();


	// For input debugging
	std::map<FixedCommand, const char*> m_pFixedCommandToString =
	{
		{COMMAND_ABILITY_ROCKET,         "Rocket"},
		{COMMAND_ABILITY_SPIKES,         "Spikes"},
		{COMMAND_ABILITY_TRAIL,          "Trail"},
		{COMMAND_CAMERA_CHANGE,          "Change Camera"},
		{COMMAND_DASH_BACK,              "Dash back"},
		{COMMAND_DASH_FORWARD,           "Dash Forward"},
		{COMMAND_DASH_LEFT,              "Dash Left"},
		{COMMAND_DASH_RIGHT,             "Dash Right"},
		{COMMAND_DEBUG_TOGGLE_WIREFRAME, "Debug Toggle Wireframe"},
		{COMMAND_MENU_BACK,              "Menu Back"},
		{COMMAND_MENU_PAUSE,             "Menu Pause"},
		{COMMAND_MENU_START,             "Menu Start"},
		{COMMAND_INVALID_FIXED,          "Invalid"},
	};

	std::map<eVariableCommand, const char*> m_pVariableCommandToString =
	{
		{COMMAND_MOVE,             "Move"},
		{COMMAND_TURN,             "Turn"},
		{COMMAND_INVALID_VARIABLE, "Invalid"},
	};

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
			return COMMAND_INVALID_FIXED;
		}
	};
	/*
	Convert a key to its corresponding eVariableCommand
	*/
	static eVariableCommand keyToVariableCommand(int key)
	{
		try
		{
			return m_pInstance->m_keyToVariableCommand.at(key);
		}
		catch (...)
		{
			return COMMAND_INVALID_VARIABLE;
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
			return COMMAND_INVALID_FIXED;
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
			return COMMAND_INVALID_FIXED;
		}
	};


	std::map<int, FixedCommand> m_keyToFixedCommand =
	{
		{GLFW_KEY_SPACE,       COMMAND_ABILITY_ROCKET},
		{GLFW_KEY_APOSTROPHE,  COMMAND_ABILITY_SPIKES},
		{GLFW_KEY_LEFT_SHIFT,  COMMAND_ABILITY_TRAIL},
		{GLFW_KEY_RIGHT_SHIFT, COMMAND_CAMERA_CHANGE},
		{GLFW_KEY_K,           COMMAND_DASH_BACK},
		{GLFW_KEY_I,           COMMAND_DASH_FORWARD},
		{GLFW_KEY_H,           COMMAND_DASH_LEFT},
		{GLFW_KEY_SEMICOLON,   COMMAND_DASH_RIGHT},
		{GLFW_KEY_F,           COMMAND_DEBUG_TOGGLE_WIREFRAME},
		{GLFW_KEY_TAB,         COMMAND_MENU_BACK},
		{GLFW_KEY_P,           COMMAND_MENU_PAUSE},
		{GLFW_KEY_ENTER,       COMMAND_MENU_START},
	};

	std::map<int, eVariableCommand> m_keyToVariableCommand =
	{
		{GLFW_KEY_W, COMMAND_MOVE},
		{GLFW_KEY_A, COMMAND_MOVE},
		{GLFW_KEY_S, COMMAND_MOVE},
		{GLFW_KEY_D, COMMAND_MOVE},
		{GLFW_KEY_J, COMMAND_TURN},
		{GLFW_KEY_L, COMMAND_TURN},
	};

	std::map<int, FixedCommand> m_buttonToFixedCommand =
	{
		{BUTTON_LEFT_BUMPER,  COMMAND_ABILITY_SPIKES},
		{BUTTON_RIGHT_BUMPER, COMMAND_CAMERA_CHANGE},
		{BUTTON_A,            COMMAND_DASH_BACK},
		{BUTTON_Y,            COMMAND_DASH_FORWARD},
		{BUTTON_X,            COMMAND_DASH_LEFT},
		{BUTTON_B,            COMMAND_DASH_RIGHT},
		{BUTTON_START,        COMMAND_DEBUG_TOGGLE_WIREFRAME},
		{BUTTON_BACK,         COMMAND_MENU_BACK},
		{BUTTON_BACK,         COMMAND_MENU_PAUSE},
		{BUTTON_START,        COMMAND_MENU_START},
		{BUTTON_LEFT_STICK,   COMMAND_INVALID_FIXED},
		{BUTTON_RIGHT_STICK,  COMMAND_INVALID_FIXED},
		{BUTTON_UP,           COMMAND_INVALID_FIXED},
		{BUTTON_RIGHT,        COMMAND_INVALID_FIXED},
		{BUTTON_DOWN,         COMMAND_INVALID_FIXED},
		{BUTTON_LEFT,         COMMAND_INVALID_FIXED},
		{BUTTON_UNKNOWN1,     COMMAND_INVALID_FIXED},
		{BUTTON_UNKNOWN2,     COMMAND_INVALID_FIXED},
	};

	std::map<int, FixedCommand> m_axisToFixedCommand =
	{
		{AXIS_LEFT_TRIGGER,  COMMAND_ABILITY_TRAIL},
		{AXIS_RIGHT_TRIGGER, COMMAND_ABILITY_ROCKET},
	};




};

