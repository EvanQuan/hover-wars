#pragma once
#include "CommandHandler.h"
#include "Mouse_Handler.h"

using namespace std;

#define MAX_INPUT_SIZE 256
#define KEYS 349

// Input Handler Class
// Handles input by storing a line of text and returning a word at a time.
class InputHandler
{
public:
	static InputHandler* getInstance(GLFWwindow *rWindow);
	~InputHandler();

	// Make private if never called externally?
	void handleKeyBoardInput(int cKey, int iAction);

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseMoveCallback(GLFWwindow* window, double x, double y);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
	// Singleton Variables
	InputHandler(GLFWwindow *rWindow);
	static InputHandler* m_pInstance;
	Mouse_Handler* m_pMouseHandler;
	CommandHandler *m_pCommandHandler;

	// Tracks all keys if they areare pressed
	// Allows for multiple key input
	// https://stackoverflow.com/questions/46631814/handling-multiple-keys-input-at-once-with-glfw
	bool pressed[KEYS];
};

