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
    // m_pGameManager = GameManager::getInstance();
    // m_pEntityManger = ENTITY_MANAGER;
    m_pInputHandler = InputHandler::getInstance(rWindow);
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
    case COMMAND_MENU_PAUSE:
    case COMMAND_MENU_START:
        // cout << "Player " << player << ": "
            // << eFixedCommandToString.at(command)
            // << endl;
       break;
    // default:
        // cout << "Player " << player << ": "
             // << eFixedCommandToString.at(command)
             // << " not implemented yet." << endl;
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
As key presses are binary, and multiple can be pressed to control x, y, x and y
must be normalized to ensure the velocity is equal to that of a joystick.
x and y as a vector should have a length of 1.
*/
void CommandHandler::normalize(float& x, float& y)
{
    float targetLength = 1;
    float magnitude = getMagnitude(x, y);
    x /= magnitude;
    y /= magnitude;
}

float CommandHandler::getMagnitude(float x, float y)
{
    return sqrt((x * x) + (y * y));
}

/*
Since checking for float equality can be messy, we need to introduce an epsilon
value.
*/
bool CommandHandler::magnitudeIsNeutral(float magnitude)
{
    float epsilon = numeric_limits<float>::epsilon(); // This is part of the Windows Library
    return (magnitude < epsilon) && (-magnitude < epsilon);
}

/*
Execute all commands specified by the keyboard.
*/
void CommandHandler::executeKeyboardCommands()
{
    bool bMovementNeutral = true;
    bool bTurnNeutral = true;
    xMove = 0;
    yMove = 0;
    xTurn = 0;
    yTurn = 0;
    for (int key = 0; key < KEYS; key++)
    {
        if (m_pInputHandler->pressed[key])
        {
            // Fixed
            switch (key)
            {
            case GLFW_KEY_K:
                m_pFixedCommand = COMMAND_DASH_BACK;
                break;
            case GLFW_KEY_I:
                m_pFixedCommand = COMMAND_DASH_FORWARD;
                break;
            case GLFW_KEY_H:
                m_pFixedCommand = COMMAND_DASH_LEFT;
                break;
            case GLFW_KEY_SEMICOLON:
                m_pFixedCommand = COMMAND_DASH_RIGHT;
                break;
            case GLFW_KEY_SPACE:
                m_pFixedCommand = COMMAND_ABILITY_ROCKET;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                m_pFixedCommand = COMMAND_ABILITY_TRAIL;
                break;
            case GLFW_KEY_APOSTROPHE:
                m_pFixedCommand = COMMAND_ABILITY_SPIKES;
                break;
            case GLFW_KEY_TAB:
                m_pFixedCommand = COMMAND_MENU_BACK;
                break;
            case GLFW_KEY_ENTER:
                m_pFixedCommand = COMMAND_MENU_START;
                break;
            case GLFW_KEY_P:
                m_pFixedCommand = COMMAND_MENU_PAUSE;
                break;
            default:
                m_pFixedCommand = COMMAND_INVALID_FIXED;
                break;
            }
            if (COMMAND_INVALID_FIXED != m_pFixedCommand)
            {
                execute(GAME_MANAGER->m_eKeyboardPlayer, m_pFixedCommand);
            }
            else
            {
                // Variable commands
                switch (key)
                {
                case GLFW_KEY_W:
                    yMove += JOYSTICK_MAX;
                    bMovementNeutral = false;
                    break;
                case GLFW_KEY_S:
                    yMove += JOYSTICK_MIN;
                    bMovementNeutral = false;
                    break;
                case GLFW_KEY_A:
                    xMove += JOYSTICK_MIN;
                    bMovementNeutral = false;
                    break;
                case GLFW_KEY_D:
                    xMove += JOYSTICK_MAX;
                    bMovementNeutral = false;
                    break;
                case GLFW_KEY_J:
                    xTurn += JOYSTICK_MIN;
                    bTurnNeutral = false;
                    break;
                case GLFW_KEY_L:
                    xTurn += JOYSTICK_MAX;
                    bTurnNeutral = false;
                    break;
                }
            }
        }
    }

    // This is where keys are handled, it's assumed that xMove and yMove will be binary on/off.
    // Let's use this assumption to our advantage and we can simply if them to the proper size instead of doing a sqrt calculation.
    if (xMove != 0.0f && yMove != 0.0f)
    {
        xMove *= 0.5f;
        yMove *= 0.5f;
    }

    if (!bMovementNeutral)//!magnitudeIsNeutral(getMagnitude(xMove, yMove)))
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
                    cout << "JUST_PRESSED" << endl;
                    execute((ePlayer) joystickID, justPressedButtonToFixedCommand(button));
                    break;
                case InputHandler::INPUT_PRESSED:
                    execute((ePlayer) joystickID, repeatButtonToFixedCommand(button));
                    break;
                case InputHandler::INPUT_JUST_RELEASED:
                    cout << "JUST_RELEASED" << endl;
                    execute((ePlayer) joystickID, justReleasedButtonToFixedCommand(button));
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
            execute((ePlayer) joystickID, COMMAND_MOVE, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
            execute((ePlayer) joystickID, COMMAND_TURN, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);
        }
    }
}
