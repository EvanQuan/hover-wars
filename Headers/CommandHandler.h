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

@author Evan Quan
*/
class CommandHandler
{
public:
    static CommandHandler* getInstance(GLFWwindow *rWindow);
    ~CommandHandler();
 
    void execute(ePlayer player, eFixedCommand command);
    void execute(ePlayer player, eVariableCommand command, float x, float y);
    /*
    Execute all commands for a given frame.
    */
    void executeAllCommands();
    /*
    Execute all commands from user input (keyboard, mouse, controllers).
    */
    void executeInputCommands();


    // For input debugging
    std::map<eFixedCommand, const char*> eFixedCommandToString =
    {
        {COMMAND_ABILITY_ROCKET,         "Rocket"},
        {COMMAND_ABILITY_SPIKES,         "Spikes"},
        {COMMAND_ABILITY_TRAIL,          "Trail"},
        {COMMAND_CAMERA_BACK,            "Camera Back"},
        {COMMAND_CAMERA_FRONT,           "Camera Front"},
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

    std::map<eVariableCommand, const char*> eVariableCommandToString =
    {
        {COMMAND_MOVE,             "Move"},
        {COMMAND_TURN,             "Turn"},
        {COMMAND_INVALID_VARIABLE, "Invalid"},
    };

private:
    // Singleton Variables
    CommandHandler(GLFWwindow *rWindow);
    static CommandHandler* m_pInstance;
    EntityManager *m_pEntityManager;
    GameManager *m_pGameManager;
    InputHandler *m_pInputHandler;
    GLFWwindow* m_pWindow;

    // Internal variables
    // For keyboard command handling
    eFixedCommand m_pFixedCommand;
    eVariableCommand m_pVariableCommand;

    vector<PlayerEntity*> m_pPlayerEntityList;
    float xMove;
    float yMove;
    float xTurn;
    float yTurn;

    void normalize(float& x, float& y);
    float getMagnitude(float x, float y);
    bool magnitudeIsNeutral(float magnitude);

    void executeKeyboardCommands();
    void executeJoystickCommands();

    void debugToggleWireframe();
    bool bWireFrameEnabled;

    /*
    Convert a pressed key to its corresponding eFixedCommand
    */
    static eFixedCommand pressedKeyToFixedCommand(int key)
    {
        try
        {
            return m_pInstance->m_pressedKeyToFixedCommand.at(key);
        }
        catch (...)
        {
            return COMMAND_INVALID_FIXED;
        }
    };
    /*
    Convert a just pressed key to its corresponding eFixedCommand
    */
    static eFixedCommand justPressedKeyToFixedCommand(int key)
    {
        try
        {
            return m_pInstance->m_justPressedKeyToFixedCommand.at(key);
        }
        catch (...)
        {
            return COMMAND_INVALID_FIXED;
        }
    };
    /*
    Convert a pressed key to its corresponding eFixedCommand
    */
    static eFixedCommand justReleasedKeyToFixedCommand(int key)
    {
        try
        {
            return m_pInstance->m_justReleasedKeyToFixedCommand.at(key);
        }
        catch (...)
        {
            return COMMAND_INVALID_FIXED;
        }
    };
    /*
    Convert a joystick button to its corresponding eFixedCommand
    if it is PRESSED OR REPEATED
    */
    static eFixedCommand repeatButtonToFixedCommand(int button)
    {
        try
        {
            return m_pInstance->m_repeatButtonToFixedCommand.at(button);
        }
        catch (...)
        {
            return COMMAND_INVALID_FIXED;
        }
    };
    /*
    Convert a joystick button to its corresponding eFixedCommand
    if it was just pressed.
    */
    static eFixedCommand justPressedButtonToFixedCommand(int button)
    {
        try
        {
            return m_pInstance->m_justPressedButtonToFixedCommand.at(button);
        }
        catch (...)
        {
            return COMMAND_INVALID_FIXED;
        }
    };
    /*
    Convert a joystick button to its corresponding eFixedCommand
    if it was just released.
    */
    static eFixedCommand justReleasedButtonToFixedCommand(int button)
    {
        try
        {
            return m_pInstance->m_justReleasedButtonToFixedCommand.at(button);
        }
        catch (...)
        {
            return COMMAND_INVALID_FIXED;
        }
    };
    /*
    Convert a joystick axis to its corresponding eFixedCommand
    */
    static eFixedCommand axisToFixedCommand(int axis)
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

    std::map<int, eFixedCommand> m_pressedKeyToFixedCommand =
    {
        {GLFW_KEY_SPACE,        COMMAND_ABILITY_ROCKET},
        {GLFW_KEY_APOSTROPHE,   COMMAND_ABILITY_SPIKES},
        {GLFW_KEY_LEFT_SHIFT,   COMMAND_ABILITY_TRAIL},
        {GLFW_KEY_K,            COMMAND_DASH_BACK},
        {GLFW_KEY_I,            COMMAND_DASH_FORWARD},
        {GLFW_KEY_H,            COMMAND_DASH_LEFT},
        {GLFW_KEY_SEMICOLON,    COMMAND_DASH_RIGHT},
        {GLFW_KEY_TAB,          COMMAND_MENU_BACK},
        {GLFW_KEY_P,            COMMAND_MENU_PAUSE},
        {GLFW_KEY_ENTER,        COMMAND_MENU_START},
        {GLFW_KEY_W,            COMMAND_MOVE_FORWARD},
        {GLFW_KEY_A,            COMMAND_MOVE_LEFT},
        {GLFW_KEY_S,            COMMAND_MOVE_BACK},
        {GLFW_KEY_D,            COMMAND_MOVE_RIGHT},
        {GLFW_KEY_J,            COMMAND_TURN_LEFT},
        {GLFW_KEY_L,            COMMAND_TURN_RIGHT},
    };

    std::map<int, eFixedCommand> m_justPressedKeyToFixedCommand =
    {
        {GLFW_KEY_ESCAPE,       COMMAND_CLOSE_WINDOW},
        {GLFW_KEY_F,            COMMAND_DEBUG_TOGGLE_WIREFRAME},
        {GLFW_KEY_1,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER1},
        {GLFW_KEY_2,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER2},
        {GLFW_KEY_3,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER3},
        {GLFW_KEY_4,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER4},
        {GLFW_KEY_C,            COMMAND_DEBUG_TOGGLE_DEBUG_CAMERA},
        {GLFW_KEY_B,            COMMAND_DEBUG_TOGGLE_SHOW_BOUNDING_BOXES},
        {GLFW_KEY_TAB,          COMMAND_MENU_BACK},
        {GLFW_KEY_RIGHT_SHIFT,  COMMAND_CAMERA_BACK},
        // 
        {GLFW_KEY_SPACE,        COMMAND_ABILITY_ROCKET},
        {GLFW_KEY_APOSTROPHE,   COMMAND_ABILITY_SPIKES},
        {GLFW_KEY_LEFT_SHIFT,   COMMAND_ABILITY_TRAIL},
        {GLFW_KEY_K,            COMMAND_DASH_BACK},
        {GLFW_KEY_I,            COMMAND_DASH_FORWARD},
        {GLFW_KEY_H,            COMMAND_DASH_LEFT},
        {GLFW_KEY_SEMICOLON,    COMMAND_DASH_RIGHT},
        {GLFW_KEY_F,            COMMAND_DEBUG_TOGGLE_WIREFRAME},
        {GLFW_KEY_TAB,          COMMAND_MENU_BACK},
        {GLFW_KEY_P,            COMMAND_MENU_PAUSE},
        {GLFW_KEY_ENTER,        COMMAND_MENU_START},
        {GLFW_KEY_W,            COMMAND_MOVE_FORWARD},
        {GLFW_KEY_A,            COMMAND_MOVE_LEFT},
        {GLFW_KEY_S,            COMMAND_MOVE_BACK},
        {GLFW_KEY_D,            COMMAND_MOVE_RIGHT},
        {GLFW_KEY_J,            COMMAND_TURN_LEFT},
        {GLFW_KEY_L,            COMMAND_TURN_RIGHT},
    };

    std::map<int, eFixedCommand> m_justReleasedKeyToFixedCommand =
    {
        {GLFW_KEY_RIGHT_SHIFT, COMMAND_CAMERA_FRONT},
    };

    std::map<int, eFixedCommand> m_repeatButtonToFixedCommand =
    {
        {BUTTON_LEFT_BUMPER,  COMMAND_ABILITY_SPIKES},
        {BUTTON_A,            COMMAND_DASH_BACK},
        {BUTTON_Y,            COMMAND_DASH_FORWARD},
        {BUTTON_X,            COMMAND_DASH_LEFT},
        {BUTTON_B,            COMMAND_DASH_RIGHT},
        {BUTTON_START,        COMMAND_DEBUG_TOGGLE_WIREFRAME},
        {BUTTON_BACK,         COMMAND_MENU_BACK},
        {BUTTON_LEFT_STICK,   COMMAND_INVALID_FIXED},
        {BUTTON_RIGHT_STICK,  COMMAND_INVALID_FIXED},
        {BUTTON_UP,           COMMAND_INVALID_FIXED},
        {BUTTON_RIGHT,        COMMAND_INVALID_FIXED},
        {BUTTON_DOWN,         COMMAND_INVALID_FIXED},
        {BUTTON_LEFT,         COMMAND_INVALID_FIXED},
        {BUTTON_UNKNOWN1,     COMMAND_INVALID_FIXED},
        {BUTTON_UNKNOWN2,     COMMAND_INVALID_FIXED},
    };

    std::map<int, eFixedCommand> m_justPressedButtonToFixedCommand =
    {
        {BUTTON_RIGHT_BUMPER, COMMAND_CAMERA_BACK},
        {BUTTON_BACK,         COMMAND_MENU_PAUSE},
        {BUTTON_START,        COMMAND_MENU_START},
        // 
        {BUTTON_LEFT_BUMPER,  COMMAND_ABILITY_SPIKES},
        {BUTTON_A,            COMMAND_DASH_BACK},
        {BUTTON_Y,            COMMAND_DASH_FORWARD},
        {BUTTON_X,            COMMAND_DASH_LEFT},
        {BUTTON_B,            COMMAND_DASH_RIGHT},
        {BUTTON_START,        COMMAND_DEBUG_TOGGLE_WIREFRAME},
        {BUTTON_BACK,         COMMAND_MENU_BACK},
        {BUTTON_LEFT_STICK,   COMMAND_INVALID_FIXED},
        {BUTTON_RIGHT_STICK,  COMMAND_INVALID_FIXED},
        {BUTTON_UP,           COMMAND_INVALID_FIXED},
        {BUTTON_RIGHT,        COMMAND_INVALID_FIXED},
        {BUTTON_DOWN,         COMMAND_INVALID_FIXED},
        {BUTTON_LEFT,         COMMAND_INVALID_FIXED},
        {BUTTON_UNKNOWN1,     COMMAND_INVALID_FIXED},
        {BUTTON_UNKNOWN2,     COMMAND_INVALID_FIXED},
    };

    std::map<int, eFixedCommand> m_justReleasedButtonToFixedCommand =
    {
        {BUTTON_RIGHT_BUMPER, COMMAND_CAMERA_FRONT},
    };

    std::map<int, eFixedCommand> m_axisToFixedCommand =
    {
        {AXIS_LEFT_TRIGGER,  COMMAND_ABILITY_TRAIL},
        {AXIS_RIGHT_TRIGGER, COMMAND_ABILITY_ROCKET},
    };

    std::map<eFixedCommand, eAbility> m_fixedCommandToAbility =
    {
        {COMMAND_ABILITY_ROCKET,    ABILITY_ROCKET},
        {COMMAND_ABILITY_SPIKES,    ABILITY_SPIKES},
        {COMMAND_ABILITY_TRAIL,     ABILITY_TRAIL},
        {COMMAND_DASH_BACK,         ABILITY_DASH_BACK},
        {COMMAND_DASH_FORWARD,      ABILITY_DASH_FORWARD},
        {COMMAND_DASH_LEFT,         ABILITY_DASH_LEFT},
        {COMMAND_DASH_RIGHT,        ABILITY_DASH_RIGHT},
    };
};

