#include "Menus/Menu.h"
#include "Menus/MenuManager.h"

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
    m_bWireFrameEnabled = false;

    m_pGameManager = GAME_MANAGER;
    m_pEntityMngr = ENTITY_MANAGER;
    m_pUserInterfaceManager = UI_MANAGER;
}

Menu::~Menu()
{
    // Nothing to delete
}

/*
Change the MenuManager's next menu
*/
void Menu::nextMenu(Menu* next)
{
    MENU_MANAGER->setCurrentMenu(next);
}

// Toggles Wireframe drawing
void Menu::debugToggleWireframe()
{
    // Toggle Boolean
    m_bWireFrameEnabled = !m_bWireFrameEnabled;

    // Set Polygon mode based on current setting.
    if (m_bWireFrameEnabled) {
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
    int iButtonCount = 0, iMask = 0, iMaskedButton = 0;
    m_pInputHandler->updateJoysticks();

    eFixedCommand command;

    for (int joystickID = GLFW_JOYSTICK_1;
        joystickID < MAX_JOYSTICK_COUNT;
        joystickID++)
    {
        bool joystickIsPresent = m_pInputHandler->isJoystickPresent(joystickID);
        if (joystickIsPresent)
        {
            const float* axes = m_pInputHandler->getAxesPointer(joystickID);
            InputHandler::eInputState *buttons = 
                m_pInputHandler->getInputStateArray(joystickID, &iButtonCount, &iMask);

            eHovercraft hovercraft = static_cast<eHovercraft>(joystickID);


            /*
                The actual button count for XBOX controllers is 14.
                As we treat the left and right triggers (which are actually axes)
                as buttons, we increase the value to 16.
                This is important for iterating through the joystick buttons
                in Menu::updateJoystickCommands() to ensure we check the trigger
                input states.
                
            */
            iButtonCount = FuncUtils::max(iButtonCount, MIN_BUTTON_COUNT);

            // Check buttons
            for (int button = 0; button < iButtonCount; button++)
            {
                iMaskedButton = iMask | button;
                switch (buttons[button])
                {
                case InputHandler::INPUT_JUST_PRESSED:
                    command = justPressedButtonToFixedCommand(iMaskedButton);
                    if (command != COMMAND_INVALID_FIXED)
                    {
                        executeFixedCommand(hovercraft, command);
                    }
                    buttons[button] = InputHandler::INPUT_PRESSED;
                    break;
                case InputHandler::INPUT_PRESSED:
                    command = repeatButtonToFixedCommand(iMaskedButton);
                    if (command != COMMAND_INVALID_FIXED)
                    {
                        executeFixedCommand(hovercraft, command);
                    }
                    break;
                case InputHandler::INPUT_JUST_RELEASED:
                    command = justReleasedButtonToFixedCommand(iMaskedButton);
                    if (command != COMMAND_INVALID_FIXED)
                    {
                        executeFixedCommand(hovercraft, command);
                    }
                    buttons[button] = InputHandler::INPUT_RELEASED;
                    break;
                }
            }

            // Check axes
            // Only update the axes values if at least one of them is not neutral.
            // This prevents unnecessary updating when both sticks are neutral.
            switch (iMask)
            {
            case PS4_MASK:
                if (axes[PS4_AXIS_LEFT_X] != 0.0f && axes[PS4_AXIS_LEFT_Y] != 0.0f)
                {
                    updateLeftStick(hovercraft, axes[PS4_AXIS_LEFT_X], -axes[PS4_AXIS_LEFT_Y]);
                }
                if (axes[PS4_AXIS_RIGHT_X] != 0.0f && axes[PS4_AXIS_RIGHT_Y] != 0.0f)
                {
                    updateRightStick(hovercraft, axes[PS4_AXIS_RIGHT_X], axes[PS4_AXIS_RIGHT_Y]);
                }
                break;
            case XBOX_MASK:
            default:
                if (axes[AXIS_LEFT_STICK_X] != 0.0f && axes[AXIS_LEFT_STICK_Y] != 0.0f)
                {
                    updateLeftStick(hovercraft, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
                }
                if (axes[AXIS_RIGHT_STICK_X] != 0.0f && axes[AXIS_RIGHT_STICK_Y] != 0.0f)
                {
                    updateRightStick(hovercraft, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);
                }
                break;
            }
        }
    }
}
