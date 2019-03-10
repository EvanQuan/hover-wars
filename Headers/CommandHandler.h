#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "SoundManager.h"

#ifndef NDEBUG
    #include "UserInterface/UserInterface.h"
#endif

/* CLASS */
/*
Receives user command initiates appropriate actions that correspond to that
command. Usually these commands would come from InputHandler (user input), but
may come from other means for test purposes.

@author Evan Quan
*/
class CommandHandler
{
public:
    static CommandHandler* getInstance(GLFWwindow *rWindow);
    ~CommandHandler();

    void executeIfHovercraftExists(eHovercraft hovercraft, eFixedCommand command);
    void executeIfHovercraftExists(eHovercraft hovercraft, eVariableCommand command, float x, float y);
    // Execute all commands for a given frame.
    void executeAllCommands();
    // Execute all commands from user input (keyboard, mouse, controllers).
    void executeInputCommands();

    // For input debugging
    unordered_map<eFixedCommand, const char*> eFixedCommandToString =
    {
        {COMMAND_ABILITY_ROCKET,         "Rocket"},
        {COMMAND_ABILITY_SPIKES,         "Spikes"},
        {COMMAND_ABILITY_TRAIL_ACTIVATE, "Trail"},
        {COMMAND_CAMERA_BACK,            "Camera Back"},
        {COMMAND_CAMERA_FRONT,           "Camera Front"},
        {COMMAND_DASH_BACK,              "Dash back"},
        {COMMAND_DASH_FORWARD,           "Dash Forward"},
        {COMMAND_DASH_LEFT,              "Dash Left"},
        {COMMAND_DASH_RIGHT,             "Dash Right"},
        {COMMAND_MENU_BACK,              "Menu Back"},
        {COMMAND_MENU_PAUSE,             "Menu Pause"},
        {COMMAND_MENU_START,             "Menu Start"},
        {COMMAND_INVALID_FIXED,          "Invalid"},
#ifndef NDEBUG

        {COMMAND_DEBUG_TOGGLE_WIREFRAME, "Debug Toggle Wireframe"},
#endif
    };

    unordered_map<eVariableCommand, const char*> eVariableCommandToString =
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
    // Only player hovercrafts receive input from the CommandHandler.
    // Bots receive input from their AI components.
    std::vector<HovercraftEntity*> players;

    void executeValidHovercraft(HovercraftEntity *hovercraft, eFixedCommand command);
    void executeValidHovercraft(HovercraftEntity *hovercraft,
                                eVariableCommand command, float x, float y);

    // Internal variables
    // For keyboard command handling
    eFixedCommand m_pFixedCommand;
    eVariableCommand m_pVariableCommand;

    void executeKeyboardCommands();
    void executeJoystickCommands();

    void debugToggleWireframe();
    bool bWireFrameEnabled;

    // Convert a pressed key to its corresponding eFixedCommand
    static eFixedCommand pressedKeyToFixedCommand(int key)
    {
        return FuncUtils::getValueIfNotDefault(m_pInstance->m_pressedKeyToFixedCommand,
                                               key, COMMAND_INVALID_FIXED);

    };
    // Convert a just pressed key to its corresponding eFixedCommand
    static eFixedCommand justPressedKeyToFixedCommand(int key)
    {
        eFixedCommand result = FuncUtils::getValueIfNotDefault(m_pInstance->m_justPressedKeyToFixedCommand,
                                                               key, COMMAND_INVALID_FIXED);
        // If the command is invalid, then check for repeat commands next
        return COMMAND_INVALID_FIXED == result ? pressedKeyToFixedCommand(key) : result;
    };
    // Convert a pressed key to its corresponding eFixedCommand
    static eFixedCommand justReleasedKeyToFixedCommand(int key)
    {
        return FuncUtils::getValueIfNotDefault(m_pInstance->m_justReleasedKeyToFixedCommand,
                                               key, COMMAND_INVALID_FIXED);
    };
    // Convert a joystick button to its corresponding eFixedCommand
    // if it is PRESSED OR REPEATED
    static eFixedCommand repeatButtonToFixedCommand(int button)
    {
        return FuncUtils::getValueIfNotDefault(m_pInstance->m_repeatButtonToFixedCommand,
                                               button, COMMAND_INVALID_FIXED);
    };
    // Convert a joystick button to its corresponding eFixedCommand
    // if it was just pressed.
    static eFixedCommand justPressedButtonToFixedCommand(int button)
    {
        eFixedCommand result = FuncUtils::getValueIfNotDefault(m_pInstance->m_justPressedButtonToFixedCommand, button, COMMAND_INVALID_FIXED);
        // If the command is invalid, then check for repeat commands next
        return COMMAND_INVALID_FIXED == result ? repeatButtonToFixedCommand(button) : result;
    };
    // Convert a joystick button to its corresponding eFixedCommand
    // if it was just released.
    static eFixedCommand justReleasedButtonToFixedCommand(int button)
    {
        return FuncUtils::getValueIfNotDefault(m_pInstance->m_justReleasedButtonToFixedCommand, button, COMMAND_INVALID_FIXED);
    };

    // These commands are issued if the player has just pressed, or is continuing
    // to press these keys
    unordered_map<int, eFixedCommand> m_pressedKeyToFixedCommand =
    {
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

    // These commands are issued only if the player has just pressed these keys
    unordered_map<int, eFixedCommand> m_justPressedKeyToFixedCommand =
    {
        {GLFW_KEY_LEFT_SHIFT,   COMMAND_ABILITY_TRAIL_ACTIVATE},
        {GLFW_KEY_SPACE,        COMMAND_ABILITY_ROCKET},
        {GLFW_KEY_LEFT_CONTROL, COMMAND_ABILITY_SPIKES},
        {GLFW_KEY_K,            COMMAND_DASH_BACK},
        {GLFW_KEY_I,            COMMAND_DASH_FORWARD},
        {GLFW_KEY_H,            COMMAND_DASH_LEFT},
        {GLFW_KEY_SEMICOLON,    COMMAND_DASH_RIGHT},
        //
        {GLFW_KEY_ESCAPE,       COMMAND_CLOSE_WINDOW},
        {GLFW_KEY_TAB,          COMMAND_MENU_BACK},
        {GLFW_KEY_COMMA,        COMMAND_CAMERA_BACK},
#ifndef NDEBUG

        {GLFW_KEY_C,            COMMAND_DEBUG_TOGGLE_DEBUG_CAMERA},
        {GLFW_KEY_F,            COMMAND_DEBUG_TOGGLE_WIREFRAME},
        {GLFW_KEY_1,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER1},
        {GLFW_KEY_2,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER2},
        {GLFW_KEY_3,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER3},
        {GLFW_KEY_4,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER4},
        {GLFW_KEY_B,            COMMAND_DEBUG_TOGGLE_DRAW_BOUNDING_BOXES},
        {GLFW_KEY_M,            COMMAND_DEBUG_TOGGLE_DRAW_SPATIAL_MAP},
        {GLFW_KEY_KP_0,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_0},
        {GLFW_KEY_KP_1,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_1},
        {GLFW_KEY_KP_2,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_2},
        {GLFW_KEY_KP_3,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_3},
        {GLFW_KEY_KP_4,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_4},
#endif
    };

    unordered_map<int, eFixedCommand> m_justReleasedKeyToFixedCommand =
    {
        {GLFW_KEY_LEFT_SHIFT,   COMMAND_ABILITY_TRAIL_DEACTIVATE},
        {GLFW_KEY_COMMA,        COMMAND_CAMERA_FRONT},
    };

    unordered_map<int, eFixedCommand> m_repeatButtonToFixedCommand =
    {
        {BUTTON_BACK,           COMMAND_MENU_BACK},
        {BUTTON_LEFT_STICK,     COMMAND_INVALID_FIXED},
        {BUTTON_RIGHT_STICK,    COMMAND_INVALID_FIXED},
        {BUTTON_UP,             COMMAND_INVALID_FIXED},
        {BUTTON_RIGHT,          COMMAND_INVALID_FIXED},
        {BUTTON_DOWN,           COMMAND_INVALID_FIXED},
        {BUTTON_LEFT,           COMMAND_INVALID_FIXED},
        {MAX_BUTTON_INDEX,      COMMAND_INVALID_FIXED},
#ifdef NDEBUG

        {BUTTON_START,          COMMAND_INVALID_FIXED},
#else
        {BUTTON_START,          COMMAND_DEBUG_TOGGLE_WIREFRAME},
#endif
    };

    unordered_map<int, eFixedCommand> m_justPressedButtonToFixedCommand =
    {
        {BUTTON_LEFT_BUMPER,    COMMAND_ABILITY_SPIKES},
        {BUTTON_A,              COMMAND_DASH_BACK},
        {BUTTON_Y,              COMMAND_DASH_FORWARD},
        {BUTTON_X,              COMMAND_DASH_LEFT},
        {BUTTON_B,              COMMAND_DASH_RIGHT},
        {BUTTON_RIGHT_BUMPER,   COMMAND_CAMERA_BACK},
        {BUTTON_BACK,           COMMAND_MENU_PAUSE},
        {BUTTON_START,          COMMAND_MENU_START},
        {BUTTON_UP,             COMMAND_HONK_UP},
        {BUTTON_RIGHT,          COMMAND_HONK_RIGHT},
        {BUTTON_DOWN,           COMMAND_HONK_DOWN},
        {BUTTON_LEFT,           COMMAND_HONK_LEFT},
        {TRIGGER_LEFT,          COMMAND_ABILITY_TRAIL_ACTIVATE},
        {TRIGGER_RIGHT,         COMMAND_ABILITY_ROCKET},
    };

    unordered_map<int, eFixedCommand> m_justReleasedButtonToFixedCommand =
    {
        {BUTTON_RIGHT_BUMPER,   COMMAND_CAMERA_FRONT},
        {TRIGGER_LEFT,          COMMAND_ABILITY_TRAIL_DEACTIVATE},
    };


    unordered_map<eFixedCommand, eAbility> m_fixedCommandToAbility =
    {
        {COMMAND_ABILITY_ROCKET,            ABILITY_ROCKET},
        {COMMAND_ABILITY_SPIKES,            ABILITY_SPIKES},
        {COMMAND_ABILITY_TRAIL_ACTIVATE,    ABILITY_TRAIL_ACTIVATE},
        {COMMAND_ABILITY_TRAIL_DEACTIVATE,  ABILITY_TRAIL_DEACTIVATE},
        {COMMAND_DASH_BACK,                 ABILITY_DASH_BACK},
        {COMMAND_DASH_FORWARD,              ABILITY_DASH_FORWARD},
        {COMMAND_DASH_LEFT,                 ABILITY_DASH_LEFT},
        {COMMAND_DASH_RIGHT,                ABILITY_DASH_RIGHT},
    };
};
