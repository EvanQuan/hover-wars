#include "CommandHandler.h"
#include "Scene_Loader.h"
#include "Physics/PhysicsManager.h"

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
Make a player of given joystickID execute a FixedCommand.
FixedCommands are binary in that either they are executed or they are not, with
no extra parameters.

For example: if a player of joystickID 0 executes the ABILITY_ROCKET command,
that is all the information the program needs to know for that player to
execute that command.
*/
void CommandHandler::execute(ePlayer player, FixedCommand command)
{
	switch (command)
	{
	case ABILITY_ROCKET:
	case ABILITY_SPIKES:
	case ABILITY_TRAIL:
	case CAMERA_CHANGE:
	case DASH_BACK:
	case DASH_FORWARD:
	case DASH_LEFT:
	case DASH_RIGHT:
	case MENU_BACK:
	case MENU_PAUSE:
	case MENU_START:
		cout << "Player " << player << ": "
		     << m_pFixedCommandToString.at(command)
		     << endl;
		break;
	default:
		cout << "Player " << player << ": "
		     << m_pFixedCommandToString.at(command)
		     << " not implemented yet." << endl;
	}
}

/*
Make a player of given joystickID execute a VariableCommand.
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
void CommandHandler::execute(ePlayer player, VariableCommand command, float x, float y)
{
	switch (command)
	{
	case MOVE:
		if (y > 0)
		{
			PHYSICS_MANAGER->forwardKey();
		}
		else if (y < 0)
		{
			PHYSICS_MANAGER->stopKey();
		}
		break;
	case TURN:
		if (x > 0)
		{
			PHYSICS_MANAGER->rightKey();
		}
		else if (x < 0)
		{
			PHYSICS_MANAGER->leftKey();
		}
		break;
	}
	std::cout << "Player " << player << ": "
		      << m_pVariableCommandToString.at(command) << std::endl
		      << "\tx: " << x << std::endl
		      << "\ty: " << y << std::endl;
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
	system("CLS"); // Clear the terminal
	executeKeyboardCommands();
	executeJoystickCommands();
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
	float epsilon = 0.001f;
	return (magnitude < epsilon) && (-magnitude < epsilon);
}

/*
Execute all commands specified by the keyboard.
*/
void CommandHandler::executeKeyboardCommands()
{
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
				m_pFixedCommand = DASH_BACK;
				break;
			case GLFW_KEY_I:
				m_pFixedCommand = DASH_FORWARD;
				break;
			case GLFW_KEY_H:
				m_pFixedCommand = DASH_LEFT;
				break;
			case GLFW_KEY_SEMICOLON:
				m_pFixedCommand = DASH_RIGHT;
				break;
			case GLFW_KEY_SPACE:
				m_pFixedCommand = ABILITY_ROCKET;
				break;
			case GLFW_KEY_LEFT_SHIFT:
				m_pFixedCommand = ABILITY_TRAIL;
				break;
			case GLFW_KEY_APOSTROPHE:
				m_pFixedCommand = ABILITY_SPIKES;
				break;
			case GLFW_KEY_TAB:
				m_pFixedCommand = MENU_BACK;
				break;
			case GLFW_KEY_ENTER:
				m_pFixedCommand = MENU_START;
				break;
			case GLFW_KEY_P:
				m_pFixedCommand = MENU_PAUSE;
				break;
			default:
				m_pFixedCommand = INVALID_FIXED;
			}
			if (INVALID_FIXED != m_pFixedCommand) {
				execute(m_pInputHandler->keyboardPlayer, m_pFixedCommand);
			}
			else
			{
				// Variable commands
				switch (key)
				{
				case GLFW_KEY_W:
					yMove += JOYSTICK_MAX;
					break;
				case GLFW_KEY_S:
					yMove += JOYSTICK_MIN;
					break;
				case GLFW_KEY_A:
					xMove += JOYSTICK_MIN;
					break;
				case GLFW_KEY_D:
					xMove += JOYSTICK_MAX;
					break;
				case GLFW_KEY_J:
					xTurn += JOYSTICK_MIN;
					break;
				case GLFW_KEY_L:
					xTurn += JOYSTICK_MAX;
					break;
				}
			}
		}
	}
	if (!magnitudeIsNeutral(getMagnitude(xMove, yMove)))
	{
		execute(m_pInputHandler->keyboardPlayer, MOVE, xMove, yMove);
	}
	if (!magnitudeIsNeutral(getMagnitude(xTurn, yTurn)))
	{
		execute(m_pInputHandler->keyboardPlayer, TURN, xTurn, yTurn);
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
			const unsigned char* buttonsPressed = m_pInputHandler->m_pJoystickButtonsPressed[joystickID];

			// Check buttons
			for (int button = BUTTON_A; button < BUTTON_UNKNOWN1; button++)
			{
				if (buttonsPressed[button])
				{
					execute((ePlayer) joystickID, buttonToFixedCommand(button));
				}
			}

			// Check axes
			// Joystick axes will not be remappable, so no need to make code generalizable
			execute((ePlayer) joystickID, MOVE, axes[AXIS_LEFT_STICK_X], axes[AXIS_LEFT_STICK_Y]);
			execute((ePlayer) joystickID, TURN, axes[AXIS_RIGHT_STICK_X], axes[AXIS_RIGHT_STICK_Y]);

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
		}
	}
}
