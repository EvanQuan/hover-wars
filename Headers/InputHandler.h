#pragma once
#include "CommandHandler.h"
#include "Mouse_Handler.h"

using namespace std;

#define MAX_INPUT_SIZE 256

// Input Handler Class
// Handles input by storing a line of text and returning a word at a time.
class InputHandler
{
public:
	static InputHandler* getInstance(GLFWwindow *rWindow);
	~InputHandler();

	void handleKeyBoardInput(int cKey, int iAction, int iMods);

private:
	// Singleton Variables
	InputHandler(GLFWwindow *rWindow);
	static InputHandler* m_pInstance;
	Mouse_Handler* m_pMouseHandler;
	CommandHandler *m_pCommandHandler;
};

