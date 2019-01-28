#pragma once
#include "CommandHandler.h"
#include "Mouse_Handler.h"

using namespace std;

#define MAX_INPUT_SIZE 256
#define KEYS 349
#define MAX_PLAYER_COUNT 4
#define KEYBOARD_PLAYER CommandHandler::PLAYER_ONE

// Input Handler Class
// Handles input by storing a line of text and returning a word at a time.
class InputHandler
{
public:
	static InputHandler* getInstance(GLFWwindow *rWindow);
	~InputHandler();

	void handleInput();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseMoveCallback(GLFWwindow* window, double x, double y);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
	// Singleton Variables
	InputHandler(GLFWwindow *rWindow);
	void initializeKeysPressed();
	void checkForPresentJoysticks();
	void initializeJoysticks();
	void initializeJoystick(int joystickID);
	static InputHandler* m_pInstance;
	Mouse_Handler* m_pMouseHandler;
	CommandHandler *m_pCommandHandler;

	// Tracks all keys if they areare pressed
	// Allows for multiple key input
	// https://stackoverflow.com/questions/46631814/handling-multiple-keys-input-at-once-with-glfw
	bool pressed[KEYS];

	// List of joysticks that are present (detected) by the game.
	// Only present controllers are initialized
	// 1 - present
	// 0 - not present
	int m_pJoystickIsPresent[MAX_PLAYER_COUNT];
	const float* m_pJoystickAxes[MAX_PLAYER_COUNT];
	const unsigned char* m_pJoystickButtons[MAX_PLAYER_COUNT];
	const char* m_pJoystickNames[MAX_PLAYER_COUNT];
};

