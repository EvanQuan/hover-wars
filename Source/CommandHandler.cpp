#include "CommandHandler.h"

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
        m_pInstance = new CommandHandler(rWindow);

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
that is all the information the program needs to know for that hovercraft to
execute that command.

@param player   to execute command on
@param command  to execute
*/
void CommandHandler::executeIfPlayerExists(ePlayer player, eFixedCommand command)
{
    if ((command == COMMAND_INVALID_FIXED) || (!ENTITY_MANAGER->playerExists(player)))
    {
        return;
    }
    PlayerEntity* playerEntity = ENTITY_MANAGER->getPlayer(player);
    execute(playerEntity, command);
}

/*
Make a bot execute a fixed command.

@param bot      to execute command on
@param command  to execute
*/
void CommandHandler::executeIfBotExists(eBot bot, eFixedCommand command)
{
    if ((command == COMMAND_INVALID_FIXED) || (!ENTITY_MANAGER->botExists(bot)))
    {
        return;
    }
    BotEntity* botEntity = ENTITY_MANAGER->getBot(bot);
    execute(botEntity, command);
}

/*
Make a hovercraft execute a fixed command.

@param hovercraft   to execute command on
@param command      to execute
*/
void CommandHandler::execute(HovercraftEntity *hovercraft, eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_ABILITY_ROCKET:
    case COMMAND_ABILITY_SPIKES:
    case COMMAND_ABILITY_TRAIL_ACTIVATE:
    case COMMAND_ABILITY_TRAIL_DEACTIVATE:
        hovercraft->useAbility(m_fixedCommandToAbility.at(command));
        break;
    case COMMAND_DASH_BACK:
        break;
    case COMMAND_DASH_FORWARD:
        break;
    case COMMAND_DASH_LEFT:
        break;
    case COMMAND_DASH_RIGHT:
        hovercraft->useAbility(m_fixedCommandToAbility.at(command));
        break;
    case COMMAND_CAMERA_FRONT:
        hovercraft->setActiveCameraToFront();
        break;
    case COMMAND_CAMERA_BACK:
        hovercraft->setActiveCameraToBack();
        break;
    case COMMAND_MENU_BACK:
       break;
    case COMMAND_MENU_PAUSE:
       break;
    case COMMAND_MENU_START:
       break;

    // Since the honk sound is played directly and all players hear the same
    // thing, we don't actually need to relate anything to the hovercraft
    // itself.
    case COMMAND_HONK_UP:
        SOUND_MANAGER->playEvent(SoundManager::SOUND_HONK_UP);
        break;
    case COMMAND_HONK_RIGHT:
        SOUND_MANAGER->playEvent(SoundManager::SOUND_HONK_RIGHT);
        break;
    case COMMAND_HONK_DOWN:
        SOUND_MANAGER->playEvent(SoundManager::SOUND_HONK_DOWN);
        break;
    case COMMAND_HONK_LEFT:
        SOUND_MANAGER->playEvent(SoundManager::SOUND_HONK_LEFT);
        break;

    case COMMAND_CLOSE_WINDOW:
        glfwSetWindowShouldClose(m_pWindow, GL_TRUE);
        break;
#ifndef NDEBUG

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
        ENTITY_MANAGER->toggleDebugCamera();
        break;
    case COMMAND_DEBUG_TOGGLE_DRAW_BOUNDING_BOXES:
        ENTITY_MANAGER->toggleBBDrawing();
        break;
    case COMMAND_DEBUG_TOGGLE_DRAW_SPATIAL_MAP:
        ENTITY_MANAGER->toggleSpatialMapDrawing();
        break;
    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_0:
        GAME_MANAGER->m_pUserInterface->setDisplayCount(0);
        break;
    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_1:
        GAME_MANAGER->m_pUserInterface->setDisplayCount(1);
        break;
    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_2:
        GAME_MANAGER->m_pUserInterface->setDisplayCount(2);
        break;
    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_3:
        GAME_MANAGER->m_pUserInterface->setDisplayCount(3);
        break;
    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_4:
        GAME_MANAGER->m_pUserInterface->setDisplayCount(4);
        break;
#endif
    }
}

/*
Make a given player execute a eVariableCommand.
VariableCommands require extra parameters to complete the command.
Specifically, they require an axis or axes to make sense of the command.

For example: if player PLAYER_1 executes the MOVE command, they also need to
specify the x and y axis values to determine what direction and at what
intensity to go at.

Axes values are normalized and follow Cartesian coordinates:
                        y = 1
                          ^
                          |
            x = -1 <-- x, y = 0 --> x = 1
                          |
                          v
                        y = -1

@param player   to execute command on
@param command  to execute
@param x        x-coordinate
@param y        y-coordinate
*/
void CommandHandler::executeIfPlayerExists(ePlayer player, eVariableCommand command, float x, float y)
{
    if ((command == COMMAND_INVALID_VARIABLE) || (!ENTITY_MANAGER->playerExists(player)))
    {
        return;
    }
    PlayerEntity* playerEntity = ENTITY_MANAGER->getPlayer(player);
    execute(playerEntity, command, x, y);
}

/*
Make a bot execute a variable command.

@param bot      to execute command on
@param command  to execute
@param x        x-coordinate
@param y        y-coordinate
*/
void CommandHandler::executeIfBotExists(eBot bot, eVariableCommand command, float x, float y)
{
    if ((command == COMMAND_INVALID_VARIABLE) || (!ENTITY_MANAGER->botExists(bot)))
    {
        return;
    }
    BotEntity* botEntity = ENTITY_MANAGER->getBot(bot);
    execute(botEntity, command, x, y);
}

/*
Make a hovercraft execute a variable command.

@param hovercraft   to execute command on
@param command      to execute
@param x            x-coordinate
@param y            y-coordinate
*/
void CommandHandler::execute(HovercraftEntity *hovercraft, eVariableCommand command, float x, float y)
{
    switch (command)
    {
    case COMMAND_MOVE:
        hovercraft->move(x, y);
        //SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_LOOP);
        break;
    case COMMAND_TURN:
        hovercraft->turn(x);
        break;
    }
}

/*
Execute all the commands for a given frame. This should be called every frame update.
*/
void CommandHandler::executeAllCommands()
{
    executeInputCommands();
    executeAllBotCommands();
}

/*
Execute all commands specified by user input from keyboard and joysticks.
*/
void CommandHandler::executeInputCommands()
{
#ifdef _DEBUG
    // system("CLS"); // Clear the terminal TODO: Delete
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
    float xMove = 0.0f;
    float yMove = 0.0f;
    float xTurn = 0.0f;
    float yTurn = 0.0f;
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
        executeIfPlayerExists(GAME_MANAGER->m_eKeyboardPlayer, m_pFixedCommand);
        // Check for cummulative movement commands, which are only executed once
        // all movement keys are checked.
        switch (m_pFixedCommand)
        {
        case COMMAND_MOVE_FORWARD:
            bMovementNeutral = false;
            yMove += JOYSTICK_IS_MAX;
            break;
        case COMMAND_MOVE_BACK:
            bMovementNeutral = false;
            yMove += JOYSTICK_IS_MIN;
            break;
        case COMMAND_MOVE_LEFT:
            bMovementNeutral = false;
            xMove += JOYSTICK_IS_MIN;
            break;
        case COMMAND_MOVE_RIGHT:
            bMovementNeutral = false;
            xMove += JOYSTICK_IS_MAX;
            break;
        case COMMAND_TURN_LEFT:
            bTurnNeutral = false;
            xTurn += JOYSTICK_IS_MIN;
            break;
        case COMMAND_TURN_RIGHT:
            bTurnNeutral = false;
            xTurn += JOYSTICK_IS_MAX;
            break;
        }
    }
    // This is where keys are handled, it's assumed that xMove and yMove will
    // be binary on/off. Let's use this assumption to our advantage and we can
    // simply if them to the proper size instead of doing a sqrt calculation.
    if ((xMove != 0.0f) && (yMove != 0.0f))
    {
        xMove *= 0.7071f;       // TODO: What is this Magic Number?
        yMove *= 0.7071f;       // TODO: What is this Magic Number?
    }

    if (!bMovementNeutral)
    {
        executeIfPlayerExists(GAME_MANAGER->m_eKeyboardPlayer, COMMAND_MOVE, xMove, yMove);
    }
    if (!bTurnNeutral)
    {
        executeIfPlayerExists(GAME_MANAGER->m_eKeyboardPlayer, COMMAND_TURN, xTurn, yTurn);
    }
}

/*
Execute all commands specified by the controllers
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
            for (int button = BUTTON_A; button < MAX_BUTTON_COUNT; button++)
            {
                switch (buttons[button])
                {
                case InputHandler::INPUT_JUST_PRESSED:
                    executeIfPlayerExists((ePlayer) joystickID, justPressedButtonToFixedCommand(button));
                    m_pInputHandler->m_joystickButtons[joystickID][button] = InputHandler::INPUT_PRESSED;
                    break;
                case InputHandler::INPUT_PRESSED:
                    executeIfPlayerExists((ePlayer) joystickID, repeatButtonToFixedCommand(button));
                    break;
                case InputHandler::INPUT_JUST_RELEASED:
                    executeIfPlayerExists((ePlayer) joystickID, justReleasedButtonToFixedCommand(button));
                    m_pInputHandler->m_joystickButtons[joystickID][button] = InputHandler::INPUT_RELEASED;
                    break;
                }
            }

            // Check axes
            // Joystick axes will not be remappable, so no need to make code generalizable
            if (axes[AXIS_LEFT_STICK_X] != 0.0f && axes[AXIS_LEFT_STICK_Y] != 0.0f)
            {
                executeIfPlayerExists((ePlayer) joystickID, COMMAND_MOVE, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
            }
            if (axes[AXIS_RIGHT_STICK_X] != 0.0f && axes[AXIS_RIGHT_STICK_Y] != 0.0f)
            {
                executeIfPlayerExists((ePlayer) joystickID, COMMAND_TURN, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);
            }
        }
    }
}

// Toggles Wireframe drawing
void CommandHandler::debugToggleWireframe()
{
    // Toggle Boolean
    bWireFrameEnabled = !bWireFrameEnabled;

    // Set Polygon mode based on current setting.
    if (bWireFrameEnabled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*
TODO
*/
void CommandHandler::executeAllBotCommands()
{

}
