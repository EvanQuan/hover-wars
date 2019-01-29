#pragma once
#include "CommandHandler.h"

#define MAX_INPUT_SIZE 256
#define KEYS 349
#define MAX_PLAYER_COUNT 4
#define KEYBOARD_PLAYER CommandHandler::PLAYER_ONE
#define XBOX_CONTROLLER "Xbox"
#define EMPTY_CONTROLLER "Empty Controller"
// m_pJoystickAxes
#define AXIS_LEFT_STICK_X 0
#define AXIS_LEFT_STICK_Y 1
#define AXIS_RIGHT_STICK_X 2
#define AXIS_RIGHT_STICK_Y 3
#define AXIS_LEFT_TRIGGER 4
#define AXIS_RIGHT_TRIGGER 5
// m_pJoystickButtons
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_X 2
#define BUTTON_Y 3
#define BUTTON_LEFT_BUMPER 4
#define BUTTON_RIGHT_BUMPER 5
#define BUTTON_BACK 6
#define BUTTON_START 7
#define BUTTON_LEFT_STICK 8
#define BUTTON_RIGHT_STICK 9
#define BUTTON_UP 10
#define BUTTON_RIGHT 11
#define BUTTON_DOWN 12
#define BUTTON_LEFT 13
// TODO What is the 16th button (15)?
// Is it the centre xbox button (home)?

// Input Handler Class
// Handles input by storing a line of text and returning a word at a time.
class InputHandler
{
public:
	static InputHandler* getInstance(GLFWwindow *rWindow);
	~InputHandler();

	void handleInput();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void joystickCallback(int joystickID, int event);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseMoveCallback(GLFWwindow* window, double x, double y);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void updateMouse(float fX, float fY);

	// Flag the Mouse for Rotations or Translations (Cannot do both at the same time).
	void mouseTStart() { m_bTranslateFlag = !m_bRotateFlag; }
	void mouseTEnd() { m_bTranslateFlag = false; }
	void mouseRStart() { m_bRotateFlag = !m_bTranslateFlag; };
	void mouseREnd() { m_bRotateFlag = false; }

	void mouseZoom(float fZoomVal);

private:
	// Singleton Variables
	InputHandler(GLFWwindow *rWindow);
	void checkForPresentJoysticks();
	void debugPrintJoystickAxes(int joystickID);
	void debugPrintJoystickButtons(int joystickID);
	void debugPrintJoystickInformation(int joystickID);
	void debugPrintJoystickInformation();
	void disconnectJoystick(int joystickID);
	void initializeJoystick(int joystickID);
	void initializeJoystickVariables();
	void initializeJoysticksAtStart();
	void initializeKeysPressed();
	void updateJoysticks();

	static InputHandler* m_pInstance;
	GameManager* m_pGameManager;
	CommandHandler* m_pCommandHandler;

	// Tracks all keys if they areare pressed
	// Allows for multiple key input
	// https://stackoverflow.com/questions/46631814/handling-multiple-keys-input-at-once-with-glfw
	bool pressed[KEYS];

	// List of joysticks that are present (detected) by the game.
	// Only present controllers are initialized
	// 1 - present
	// 0 - not present
	int m_pJoystickIsPresent[MAX_PLAYER_COUNT];
	int m_pJoystickAxesCount[MAX_PLAYER_COUNT];
	const float* m_pJoystickAxes[MAX_PLAYER_COUNT];
	int m_pJoystickButtonCount[MAX_PLAYER_COUNT];
	const unsigned char* m_pJoystickButtons[MAX_PLAYER_COUNT];
	const char* m_pJoystickNames[MAX_PLAYER_COUNT];

	// Mouse
	bool m_bTranslateFlag;
	bool m_bRotateFlag;
	glm::vec2 m_pInitialPos;
};

