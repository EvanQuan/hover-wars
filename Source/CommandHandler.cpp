#include "CommandHandler.h"
#include "SceneLoader.h"

// Singleton instance
CommandHandler* CommandHandler::m_pInstance = nullptr;
/*
Constructor
*/
CommandHandler::CommandHandler(GLFWwindow *rWindow)
{
    // Initializing Base Class
    // NOTE: Do not get an instance of GameManager here or there will be
    // infinite mutual recursion and a call stack overflow
    m_pWindow = rWindow;
    m_pInputHandler = InputHandler::getInstance(rWindow);
    bWireFrameEnabled = false;
}

/*
Get Singleton instance
*/
CommandHandler* CommandHandler::getInstance(GLFWwindow *rWindow)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new CommandHandler(rWindow);
    }

    return m_pInstance;
}

/*
Destructor
*/
CommandHandler::~CommandHandler()
{
    // m_pEntMngr = nullptr;
    // NOTE: These are deleted in main
    m_pInputHandler = nullptr;
    m_pGameManager = nullptr;
}

/*
Make a player execute a eFixedCommand.
FixedCommands are binary in that either they are executed or they are not, with
no extra parameters.

For example: if a player PLAYER_2 executes the ABILITY_ROCKET command,
that is all the information the program needs to know for that player to
execute that command.
*/
void CommandHandler::execute(ePlayer player, eFixedCommand command)
{
    if (!ENTITY_MANAGER->playerExists(player))
    {
        return;
    }
    switch (command)
    {
    case COMMAND_ABILITY_ROCKET:
    case COMMAND_ABILITY_SPIKES:
    case COMMAND_ABILITY_TRAIL:
        if (ENTITY_MANAGER->playerExists(player))
            ENTITY_MANAGER->getPlayer(player)->useAbility(m_fixedCommandToAbility.at(command));
        break;
    case COMMAND_DASH_BACK:
        break;
    case COMMAND_DASH_FORWARD:
        break;
    case COMMAND_DASH_LEFT:
        break;
    case COMMAND_DASH_RIGHT:
        ENTITY_MANAGER->getPlayer(player)->useAbility(m_fixedCommandToAbility.at(command));
        break;
    case COMMAND_CAMERA_FRONT:
        ENTITY_MANAGER->getPlayer(player)->setActiveCameraToFront();
        break;
    case COMMAND_CAMERA_BACK:
        ENTITY_MANAGER->getPlayer(player)->setActiveCameraToBack();
        break;
    case COMMAND_MENU_BACK:
       break;
    case COMMAND_MENU_PAUSE:
       break;
    case COMMAND_MENU_START:
       break;

    case COMMAND_CLOSE_WINDOW:
        glfwSetWindowShouldClose(m_pWindow, GL_TRUE);
        break;
    case COMMAND_DEBUG_TOGGLE_WIREFRAME:
        debugToggleWireframe();
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER1:
        GAME_MANAGER->m_eKeyboardPlayer = PLAYER_1;
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER2:
        GAME_MANAGER->m_eKeyboardPlayer = PLAYER_2;
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER3:
        GAME_MANAGER->m_eKeyboardPlayer = PLAYER_3;
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER4:
        GAME_MANAGER->m_eKeyboardPlayer = PLAYER_4;
        break;
    case COMMAND_DEBUG_TOGGLE_DEBUG_CAMERA:
        GAME_MANAGER->toggleDebugCamera();
        break;
    case COMMAND_DEBUG_TOGGLE_DRAW_BOUNDING_BOXES:
        ENTITY_MANAGER->toggleBBDrawing();
        break;
    case COMMAND_DEBUG_TOGGLE_DRAW_SPATIAL_MAP:
        ENTITY_MANAGER->toggleSpatialMapDrawing();
        break;
    }
}

/*
Make a player of given joystickID execute a eVariableCommand.
VariableCommands require extra parameters to complete the command.
Specifically, they require an axis or axes to make sense of the command.

For example: if a player of joystickID 0 executes the MOVE command, they also
need to specify the x and y axis values to determine what direction and at what
intensity to go at.

Axes values are normalized and follow Cartesian coordinates:
                        y = 1
                          ^
                          |
            x = -1 <-- x, y = 0 --> x = 1
                          |
                          v
                        y = -1
*/
void CommandHandler::execute(ePlayer player, eVariableCommand command, float x, float y)
{
    if (ENTITY_MANAGER->playerExists(player))
    {
        switch (command)
        {
        case COMMAND_MOVE:
            ENTITY_MANAGER->getPlayer(player)->move(x, y);
            SOUND_MANAGER->playSounds("Sound/car_start.wav", vec3(0, 0, 0), SOUND_MANAGER->volumeTodB(1.0f));
            break;
        case COMMAND_TURN:
            ENTITY_MANAGER->getPlayer(player)->turn(x);
            break;
        }
    }

#ifdef _DEBUG
    // std::cout << "Player " << player << ": "
    //           << eVariableCommandToString.at(command) << std::endl
    //           << "\tx: " << x << std::endl
    //           << "\ty: " << y << std::endl;
#endif
}

/*
Execute all the commands for a given frame. This should be called every frame update.
*/
void CommandHandler::executeAllCommands()
{
    executeInputCommands();
}

/*
Execute all commands specified by user input from keyboard and joysticks.
*/
void CommandHandler::executeInputCommands()
{
#ifdef _DEBUG
    // system("CLS"); // Clear the terminal
#endif
    executeJoystickCommands();
    executeKeyboardCommands();
}

/*
Execute all commands specified by the keyboard.
*/
void CommandHandler::executeKeyboardCommands()
{
    bool bMovementNeutral = true;
    bool bTurnNeutral = true;
    float xMove, yMove, xTurn, yTurn = 0;
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
    for (auto it : keys)
    {
        /*
        Divide the key states into the 3 types of fixed commands
        */
        switch (it.second) // value - input state
        {
        case InputHandler::INPUT_JUST_PRESSED:
            /*
            Just pressed now changed to pressed, as if the key is continued to
            be pressed next frame, it should read as a key repeat.
            */
            m_pInputHandler->m_keys[it.first] = InputHandler::INPUT_PRESSED;
            m_pFixedCommand = justPressedKeyToFixedCommand(it.first);
            break;
        case InputHandler::INPUT_PRESSED:
            m_pFixedCommand = pressedKeyToFixedCommand(it.first);
            break;
        case InputHandler::INPUT_JUST_RELEASED:
            /*
            Now that the key ha been read as just released, we can now remove
            it, so it doesn't need to be iterated over again next frame.
            */
            m_pInputHandler->m_keys.erase(it.first);
            m_pFixedCommand = justReleasedKeyToFixedCommand(it.first);
            break;
        }
        execute(GAME_MANAGER->m_eKeyboardPlayer, m_pFixedCommand);
        // Check for cummulative movement commands, which are only executed once
        // all movement keys are checked.
        switch (m_pFixedCommand)
        {
        case COMMAND_MOVE_FORWARD:
            bMovementNeutral = false;
            yMove += JOYSTICK_MAX;
            break;
        case COMMAND_MOVE_BACK:
            bMovementNeutral = false;
            yMove += JOYSTICK_MIN;
            break;
        case COMMAND_MOVE_LEFT:
            bMovementNeutral = false;
            xMove += JOYSTICK_MIN;
            break;
        case COMMAND_MOVE_RIGHT:
            bMovementNeutral = false;
            xMove += JOYSTICK_MAX;
            break;
        case COMMAND_TURN_LEFT:
            bTurnNeutral = false;
            xTurn += JOYSTICK_MIN;
            break;
        case COMMAND_TURN_RIGHT:
            bTurnNeutral = false;
            xTurn += JOYSTICK_MAX;
            break;
        }
    }
    // This is where keys are handled, it's assumed that xMove and yMove will
    // be binary on/off. Let's use this assumption to our advantage and we can
    // simply if them to the proper size instead of doing a sqrt calculation.
    if (xMove != 0.0f && yMove != 0.0f)
    {
        xMove *= 0.5f;
        yMove *= 0.5f;
    }

    if (!bMovementNeutral)
    {
        execute(GAME_MANAGER->m_eKeyboardPlayer, COMMAND_MOVE, xMove, yMove);
    }
    if (!bTurnNeutral)
    {
        execute(GAME_MANAGER->m_eKeyboardPlayer, COMMAND_TURN, xTurn, yTurn);
    }
}

/*
Execute all commands specified by the controllers

@TODO this needs to be changed to factor in keyboard input rework.
*/
void CommandHandler::executeJoystickCommands()
{
    m_pInputHandler->updateJoysticks();

    for (int joystickID = GLFW_JOYSTICK_1; joystickID < MAX_PLAYER_JOYSTICK; joystickID++)
    {
        int joystickIsPresent = m_pInputHandler->m_pJoystickIsPresent[joystickID];
        if (joystickIsPresent)
        {
            const float* axes = m_pInputHandler->m_pJoystickAxes[joystickID];
            InputHandler::eInputState *buttons = m_pInputHandler->m_joystickButtons[joystickID];

            // Check buttons
            for (int button = BUTTON_A; button < BUTTON_UNKNOWN1; button++)
            {
                switch (buttons[button])
                {
                case InputHandler::INPUT_JUST_PRESSED:
                    execute((ePlayer) joystickID, justPressedButtonToFixedCommand(button));
                    m_pInputHandler->m_joystickButtons[joystickID][button] = InputHandler::INPUT_PRESSED;
                    break;
                case InputHandler::INPUT_PRESSED:
                    execute((ePlayer) joystickID, repeatButtonToFixedCommand(button));
                    break;
                case InputHandler::INPUT_JUST_RELEASED:
                    execute((ePlayer) joystickID, justReleasedButtonToFixedCommand(button));
                    m_pInputHandler->m_joystickButtons[joystickID][button] = InputHandler::INPUT_RELEASED;
                    break;
                }
            }

            // NOTE: With works with the assumption that triggers are mapped to fixed commands
            // If we decide that triggers work better for variable commands, then we will need to change this.
            if (axes[AXIS_LEFT_TRIGGER] > TRIGGER_NETURAL)
            {
                execute((ePlayer) joystickID, axisToFixedCommand(AXIS_LEFT_TRIGGER));
            }
            if (axes[AXIS_RIGHT_TRIGGER] > TRIGGER_NETURAL)
            {
                execute((ePlayer) joystickID, axisToFixedCommand(AXIS_RIGHT_TRIGGER));
            }

            // Check axes
            // Joystick axes will not be remappable, so no need to make code generalizable
            if (axes[AXIS_LEFT_STICK_X] != 0.0f && axes[AXIS_LEFT_STICK_Y] != 0.0f)
            {
                execute((ePlayer) joystickID, COMMAND_MOVE, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
            }
            if (axes[AXIS_RIGHT_STICK_X] != 0.0f && axes[AXIS_RIGHT_STICK_Y] != 0.0f)
            {
                execute((ePlayer) joystickID, COMMAND_TURN, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);
            }
        }
    }
}

void CommandHandler::debugToggleWireframe()
{
    bWireFrameEnabled = !bWireFrameEnabled;
    if (bWireFrameEnabled)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}
