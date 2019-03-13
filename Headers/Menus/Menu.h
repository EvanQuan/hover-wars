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
Menus determine the state of the game with respect to how input is processed by
the CommandHandler.

Depending on the current menu state of the CommandHander, the input is processed
differently.

Every subclass should add its singleton instance to CommandHandler's
menuInstances vector.

@author Evan Quan
*/
class Menu abstract
{
public:
    ~Menu();

    virtual void update() final { updateKeyboardCommands(); updateJoystickCommands(); };

private:
    bool bWireFrameEnabled;

    unordered_map<eVariableCommand, const char*> eVariableCommandToString;
    unordered_map<int, eFixedCommand> m_pressedKeyToFixedCommand;
    unordered_map<int, eFixedCommand> m_justPressedKeyToFixedCommand;
    unordered_map<int, eFixedCommand> m_justReleasedKeyToFixedCommand;
    unordered_map<int, eFixedCommand> m_repeatButtonToFixedCommand;
    unordered_map<int, eFixedCommand> m_justPressedButtonToFixedCommand;
    unordered_map<int, eFixedCommand> m_justReleasedButtonToFixedCommand;
protected:
    Menu(
        unordered_map<eVariableCommand, const char*> eVariableCommandToString,
        unordered_map<int, eFixedCommand> m_pressedKeyToFixedCommand,
        unordered_map<int, eFixedCommand> m_justPressedKeyToFixedCommand,
        unordered_map<int, eFixedCommand> m_justReleasedKeyToFixedCommand,
        unordered_map<int, eFixedCommand> m_repeatButtonToFixedCommand,
        unordered_map<int, eFixedCommand> m_justPressedButtonToFixedCommand,
        unordered_map<int, eFixedCommand> m_justReleasedButtonToFixedCommand
    );

    void nextMenu(Menu* next);

    virtual void updateKeyboardCommands() = 0;
    virtual void updateJoystickCommands() = 0;

    InputHandler *m_pInputHandler;

    // Only player hovercrafts receive input from the Menu.
    // Bots receive input from their AI components.
    std::vector<HovercraftEntity*> players;

    // Toggles Wireframe drawing
    void Menu::debugToggleWireframe();

    // Convert a pressed key to its corresponding eFixedCommand
    eFixedCommand pressedKeyToFixedCommand(int key)
    {
        return FuncUtils::getValueIfNotDefault(m_pressedKeyToFixedCommand,
                                               key, COMMAND_INVALID_FIXED);

    };
    // Convert a just pressed key to its corresponding eFixedCommand
    eFixedCommand justPressedKeyToFixedCommand(int key)
    {
        eFixedCommand result = FuncUtils::getValueIfNotDefault(m_justPressedKeyToFixedCommand,
                                                               key, COMMAND_INVALID_FIXED);
        // If the command is invalid, then check for repeat commands next
        return COMMAND_INVALID_FIXED == result ? pressedKeyToFixedCommand(key) : result;
    };
    // Convert a pressed key to its corresponding eFixedCommand
    eFixedCommand justReleasedKeyToFixedCommand(int key)
    {
        return FuncUtils::getValueIfNotDefault(m_justReleasedKeyToFixedCommand,
                                               key, COMMAND_INVALID_FIXED);
    };
    // Convert a joystick button to its corresponding eFixedCommand
    // if it is PRESSED OR REPEATED
    eFixedCommand repeatButtonToFixedCommand(int button)
    {
        return FuncUtils::getValueIfNotDefault(m_repeatButtonToFixedCommand,
                                               button, COMMAND_INVALID_FIXED);
    };
    // Convert a joystick button to its corresponding eFixedCommand
    // if it was just pressed.
    eFixedCommand justPressedButtonToFixedCommand(int button)
    {
        eFixedCommand result = FuncUtils::getValueIfNotDefault(m_justPressedButtonToFixedCommand, button, COMMAND_INVALID_FIXED);
        // If the command is invalid, then check for repeat commands next
        return COMMAND_INVALID_FIXED == result ? repeatButtonToFixedCommand(button) : result;
    };
    // Convert a joystick button to its corresponding eFixedCommand
    // if it was just released.
    eFixedCommand justReleasedButtonToFixedCommand(int button)
    {
        return FuncUtils::getValueIfNotDefault(m_justReleasedButtonToFixedCommand, button, COMMAND_INVALID_FIXED);
    };
};
