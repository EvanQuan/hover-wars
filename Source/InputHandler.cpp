#include <iostream> // debug
#include "stdafx.h"
#include "InputHandler.h"

// Single Singleton instance
InputHandler* InputHandler::m_pInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *rWindow)
{
	// Keyboard
	m_keyboardPlayer = GLFW_JOYSTICK_1;
	initializeKeysPressed();
	glfwSetKeyCallback(rWindow, InputHandler::keyCallback);
	// Mouse
	// m_bRotateFlag = m_bTranslateFlag = false;
	// glfwSetMouseButtonCallback(rWindow, InputHandler::mouseButtonCallback);
	// glfwSetCursorPosCallback(rWindow, InputHandler::mouseMoveCallback);
	// glfwSetScrollCallback(rWindow, InputHandler::mouseScrollCallback);
	// Controller
	initializeJoysticksAtStart();
	glfwSetJoystickCallback(InputHandler::joystickCallback);


}

InputHandler* InputHandler::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new InputHandler(rWindow);
	}

	return m_pInstance;
}

InputHandler::~InputHandler()
{
	// m_pGameManager = nullptr; // TODO remove later once GameManager no longer needed
}


// handles keyboard input events
void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (GLFW_KEY_UNKNOWN == key)
	{
		return;
	}

	// Possible actions:
	//		GLFW_RELEASE = 0
	//		GLFW_PRESS   = 1
	//		GLFW_REPEAT  = 2
	// Note that any time a key is pressed, it will count as TRUE
	m_pInstance->pressed[key] = action;

	// Special keys handled differently than just pressed/not pressed
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		// TODO Later should escape no longer close window, as there may need
		// to be some tear down steps before the game exits?
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_F:
		if (GLFW_PRESS == action)
		{
			// m_pInstance->m_pCommandHandler->execute(m_pInstance->m_keyboardPlayer, CommandHandler::DEBUG_TOGGLE_WIREFRAME);
		}
		break;

	}
}

// Mouse Button Callback
// Handle mouse movement controls.
// void InputHandler::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
// {
	// double fX, fY;
// 
	// if (GLFW_MOUSE_BUTTON_1 == button)
	// {
		// glfwGetCursorPos(window, &fX, &fY);
		// if (GLFW_PRESS == action)
		// {
			// m_pInstance->mouseTStart();
		// }
		// else if (GLFW_RELEASE == action)
		// {
			// m_pInstance->mouseTEnd();
		// }
	// }
	// if (GLFW_MOUSE_BUTTON_2 == button)
	// {
		// glfwGetCursorPos(window, &fX, &fY);
		// if (GLFW_PRESS == action)
		// {
			// m_pInstance->mouseRStart();
		// }
		// else if (GLFW_RELEASE == action)
		// {
			// m_pInstance->mouseREnd();
		// }
	// }
// }

// Handles input from Mouse Moves.
// void InputHandler::mouseMoveCallback(GLFWwindow* window, double x, double y)
// {
	// if (m_pInstance->m_bRotateFlag)
	// {
		// m_pInstance->m_pGameManager->rotateCamera(m_pInstance->m_pInitialPos - vec2((float) x, (float) y));
	// }
// 
	// // Set new current position
	// m_pInstance->m_pInitialPos.x = (float) x;
	// m_pInstance->m_pInitialPos.y = (float) y;
// }

// Handle scroll wheel callbacks
// void InputHandler::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
// {
	// m_pInstance->m_pGameManager->zoomCamera((float) yoffset * 0.05f);
// }

// Keys begin not pressed until notified that they are by keyCallback.
void InputHandler::initializeKeysPressed()
{
	for (int key = 0; key < KEYS; key++)
	{
		pressed[key] = false;
	}
}

// TODO later this should detect controllers connecting/disconnecting in the
// middle of the menu or game.
void InputHandler::initializeJoysticksAtStart()
{
	initializeJoystickVariables();
	checkForPresentJoysticks();

	// Initialize all connnected joysticks
	// joystickCallback only checks for joysticks that go from disconnected to
	// connected, and therefore does not check for joysticks that are already
	// connected at the start of the program.
	for (int joystickID = GLFW_JOYSTICK_1; joystickID < MAX_PLAYER_COUNT; joystickID++)
	{
		initializeJoystick(joystickID);
	}
}

// Initialize joystick variables before they are set
void InputHandler::initializeJoystickVariables()
{
	for (int i = 0; i < MAX_PLAYER_COUNT; i++)
	{
		m_pJoystickIsPresent[i] = false;
		m_pJoystickAxesCount[i] = 0;
		m_pJoystickButtonCount[i] = 0;
		m_pJoystickNames[i] = EMPTY_CONTROLLER;
	}
}

// Check all if all joysticks are connected
void InputHandler::checkForPresentJoysticks()
{
	for (int i = 0; i < MAX_PLAYER_COUNT; i++)
	{
		m_pJoystickIsPresent[i] = glfwJoystickPresent(i);
	}
}


// This should be used instead of initializeJoysticks due to how
// controllers connecting and disconnecting can work
// It should also check if a controller is already initialized before doing so
// but need to emperically check if ID changes when controllers connect/disconnect
// before doing so
void InputHandler::initializeJoystick(int joystickID)
{
	// Check if joystick is present. Can only initialize if present.
	m_pJoystickIsPresent[joystickID] = glfwJoystickPresent(joystickID);
	if (!m_pJoystickIsPresent[joystickID])
	{
		return;
	}
	// Axis states
	m_pJoystickAxes[joystickID] = glfwGetJoystickAxes(joystickID, &m_pJoystickAxesCount[joystickID]);

	// Button states
	m_pJoystickButtonsPressed[joystickID] = glfwGetJoystickButtons(joystickID, &m_pJoystickButtonCount[joystickID]);

	// Names
	m_pJoystickNames[joystickID] = glfwGetJoystickName(joystickID);

	debugPrintJoystickInformation(joystickID);
}

// DEBUG Print information about all joysticks
void InputHandler::debugPrintJoystickInformation()
{
	return; // DEBUG
	system("CLS");
	for (int joystickID = 0; joystickID < MAX_PLAYER_COUNT; joystickID++)
	{
		debugPrintJoystickInformation(joystickID);
	}

}

// DEBUG Print information about a joystick
void InputHandler::debugPrintJoystickInformation(int joystickID)
{
	std::cout << m_pJoystickNames[joystickID] << " " << joystickID
		<< " is " <<  (m_pJoystickIsPresent[joystickID] ? "" : "dis" ) << "connected"
		<< std::endl;

	debugPrintJoystickAxes(joystickID);
	debugPrintJoystickButtons(joystickID);
}

void InputHandler::debugPrintJoystickAxes(int joystickID)
{

	if (!m_pJoystickIsPresent[joystickID])
	{
		return;
	}
	const float* axes = m_pJoystickAxes[joystickID];
	std::cout << "\tAxes[" << m_pJoystickAxesCount[joystickID] << "]: " << std::endl
	          << "\t\tLeft stick x: " << axes[AXIS_LEFT_STICK_X] << std::endl
	          << "\t\t           y: " << axes[AXIS_LEFT_STICK_Y] << std::endl
	          << "\t\tRight stick x: " << axes[AXIS_RIGHT_STICK_X] << std::endl
	          << "\t\t            y: " << axes[AXIS_RIGHT_STICK_Y] << std::endl
	          << "\t\tLeft trigger: " << axes[AXIS_LEFT_TRIGGER] << std::endl
	          << "\t\tRight trigger: " << axes[AXIS_RIGHT_TRIGGER] << std::endl;
	std::cout << "\t\t[";
	for (int i = 0; i < m_pJoystickAxesCount[joystickID]; i++)
	{
		std::cout << axes[i] << " ";
	}
	std::cout << "]" << std::endl;

}

void InputHandler::debugPrintJoystickButtons(int joystickID)
{
	if (!m_pJoystickIsPresent[joystickID])
	{
		return;
	}
	const unsigned char* buttonsPressed = m_pJoystickButtonsPressed[joystickID];
	std::cout << "\tButtons[" << m_pJoystickButtonCount[joystickID] << "]: " << std::endl
	          << "\t\tA: " << buttonsPressed[BUTTON_A] << std::endl
	          << "\t\tB: " << buttonsPressed[BUTTON_B] << std::endl
	          << "\t\tX: " << buttonsPressed[BUTTON_X] << std::endl
	          << "\t\tY: " << buttonsPressed[BUTTON_Y] << std::endl
	          << "\t\tLeft Bumper: " << buttonsPressed[BUTTON_LEFT_BUMPER] << std::endl
	          << "\t\tRight Bumper: " << buttonsPressed[BUTTON_RIGHT_BUMPER] << std::endl
	          << "\t\tBack: " << buttonsPressed[BUTTON_BACK] << std::endl
	          << "\t\tStart: " << buttonsPressed[BUTTON_START] << std::endl
	          << "\t\tLeft Stick: " << buttonsPressed[BUTTON_LEFT_STICK] << std::endl
	          << "\t\tRight Stick: " << buttonsPressed[BUTTON_RIGHT_STICK] << std::endl
	          << "\t\tUp: " << buttonsPressed[BUTTON_UP] << std::endl
	          << "\t\tRight: " << buttonsPressed[BUTTON_RIGHT] << std::endl
	          << "\t\tDown: " << buttonsPressed[BUTTON_DOWN] << std::endl
	          << "\t\tLeft: " << buttonsPressed[BUTTON_LEFT] << std::endl;
	std::cout << "\t\t[";
	for (int i = 0; i < m_pJoystickButtonCount[joystickID]; i++)
	{
		std::cout << buttonsPressed[i] << " ";
	}
	std::cout << "]" << std::endl;

}

void InputHandler::disconnectJoystick(int joystickID)
{
	m_pJoystickIsPresent[joystickID] = false;
	debugPrintJoystickInformation(joystickID);

}

// Checks for joystick connection/disconnection
// TODO true? Must be called in initialize() to avoid infinite getInstance() recursion
void InputHandler::joystickCallback(int joystickID, int event)
{
	if (event == GLFW_CONNECTED)
	{
		m_pInstance->initializeJoystick(joystickID);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		m_pInstance->disconnectJoystick(joystickID);
	}
}

// Get the input status of all the present controllers and store it.
// Buttons and axes
// TODO more efficient iteration algorithm?
void InputHandler::updateJoysticks()
{
	for (int joystickID = GLFW_JOYSTICK_1; joystickID < MAX_PLAYER_COUNT; joystickID++)
	{
		if (m_pJoystickIsPresent[joystickID])
		{
			// Axis states
			m_pJoystickAxes[joystickID] = glfwGetJoystickAxes(joystickID, &m_pJoystickAxesCount[joystickID]);

			// Button states
			m_pJoystickButtonsPressed[joystickID] = glfwGetJoystickButtons(joystickID, &m_pJoystickButtonCount[joystickID]);
		}
	}

}