#include "Menus/Menu.h"
#include "CommandHandler.h"

Menu::Menu(
    unordered_map<int, eFixedCommand> pressedKeyToFixedCommand,
    unordered_map<int, eFixedCommand> justPressedKeyToFixedCommand,
    unordered_map<int, eFixedCommand> justReleasedKeyToFixedCommand,
    unordered_map<int, eFixedCommand> repeatButtonToFixedCommand,
    unordered_map<int, eFixedCommand> justPressedButtonToFixedCommand,
    unordered_map<int, eFixedCommand> justReleasedButtonToFixedCommand
)
{
    m_pressedKeyToFixedCommand         = pressedKeyToFixedCommand;
    m_justPressedKeyToFixedCommand     = justPressedKeyToFixedCommand;
    m_justReleasedKeyToFixedCommand    = justReleasedKeyToFixedCommand;
    m_repeatButtonToFixedCommand       = repeatButtonToFixedCommand;
    m_justPressedButtonToFixedCommand  = justPressedButtonToFixedCommand;
    m_justReleasedButtonToFixedCommand = justReleasedButtonToFixedCommand;

    m_pInputHandler = InputHandler::getInstance();
    bWireFrameEnabled = false;

    m_pGameManager = GameManager::getInstance();
    m_pEntityMngr = ENTITY_MANAGER;
}

Menu::~Menu()
{
    // Nothing to delete
}

/*
Change the CommandHandler's next menu
*/
void Menu::nextMenu(Menu* next)
{
    COMMAND_HANDLER->setCurrentMenu(next);
}

// Toggles Wireframe drawing
void Menu::debugToggleWireframe()
{
    // Toggle Boolean
    bWireFrameEnabled = !bWireFrameEnabled;

    // Set Polygon mode based on current setting.
    if (bWireFrameEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/*
Execute all commands specified by the keyboard.
*/
void Menu::updateKeyboardCommands()
{
    setupKeyCommands();
    /*
    Copy the keys at the current snapshot so they can be iterated over while
    m_keys continues to be updated.

    NOTE: Unless there is some other better way of doing this, directly using
    m_pInputHandler->m_keys's iterator will crash, as the map is continuing to
    update through key callbacks as it is being iterated through.

    Considering that keys will typically contain 0-4 elements, this should be
    fairly cheap.
    */
    map<int, InputHandler::eInputState> keys = m_pInputHandler->m_keys;
    eFixedCommand fixedCommand = COMMAND_INVALID_FIXED;
   for (auto it : keys)
    {
        // Divide the key states into the 3 types of fixed commands
        switch (it.second) // value - input state
        {
        case InputHandler::INPUT_JUST_PRESSED:
            /*
            Just pressed now changed to pressed, as if the key is continued to
            be pressed next frame, it should read as a key repeat.
            */
            m_pInputHandler->m_keys[it.first] = InputHandler::INPUT_PRESSED;
            fixedCommand = justPressedKeyToFixedCommand(it.first);
            break;
        case InputHandler::INPUT_PRESSED:
            fixedCommand = pressedKeyToFixedCommand(it.first);
            break;
        case InputHandler::INPUT_JUST_RELEASED:
            /*
            Now that the key ha been read as just released, we can now remove
            it, so it doesn't need to be iterated over again next frame.
            */
            m_pInputHandler->m_keys.erase(it.first);
            fixedCommand = justReleasedKeyToFixedCommand(it.first);
            break;
        }
        executeFixedCommand(GAME_MANAGER->getKeyboardHovercraft(), fixedCommand);
        handleAccumulatedKeyCommands(fixedCommand);
    }
    executeAccumulatedKeyCommands(GAME_MANAGER->getKeyboardHovercraft());
}

void Menu::updateJoystickCommands()
{
    m_pInputHandler->updateJoysticks();

    eFixedCommand command;

    for (int joystickID = GLFW_JOYSTICK_1;
        joystickID < MAX_PLAYER_JOYSTICK;
        joystickID++)
    {
        int joystickIsPresent = m_pInputHandler->m_pJoystickIsPresent[joystickID];
        if (joystickIsPresent)
        {
            const float* axes = m_pInputHandler->m_pJoystickAxes[joystickID];
            InputHandler::eInputState *buttons =
                m_pInputHandler->m_joystickButtons[joystickID];

            eHovercraft hovercraft = static_cast<eHovercraft>(joystickID);

            // Check buttons
            for (int button = BUTTON_A; button < MAX_BUTTON_COUNT; button++)
            {
                switch (buttons[button])
                {
                case InputHandler::INPUT_JUST_PRESSED:
                    command = justPressedButtonToFixedCommand(button);
                    if (command != COMMAND_INVALID_FIXED)
                    {
                        executeFixedCommand(hovercraft, command);
                    }
                    m_pInputHandler->m_joystickButtons[joystickID][button] =
                        InputHandler::INPUT_PRESSED;
                    break;
                case InputHandler::INPUT_PRESSED:
                    command = repeatButtonToFixedCommand(button);
                    if (command != COMMAND_INVALID_FIXED)
                    {
                        executeFixedCommand(hovercraft, command);
                    }
                    break;
                case InputHandler::INPUT_JUST_RELEASED:
                    command = justReleasedButtonToFixedCommand(button);
                    if (command != COMMAND_INVALID_FIXED)
                    {
                        executeFixedCommand(hovercraft, command);
                    }
                    m_pInputHandler->m_joystickButtons[joystickID][button] =
                        InputHandler::INPUT_RELEASED;
                    break;
                }
            }

            // Check axes
            if (axes[AXIS_LEFT_STICK_X] != 0.0f && axes[AXIS_LEFT_STICK_Y] != 0.0f)
            {
                updateLeftStick(hovercraft, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
            }
            if (axes[AXIS_RIGHT_STICK_X] != 0.0f && axes[AXIS_RIGHT_STICK_Y] != 0.0f)
            {
                updateRightStick(hovercraft, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);
            }
        }
    }
}
