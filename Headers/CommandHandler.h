#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "InputHandler.h"
#include "GameManager.h"
#include "EntityManager.h"

/* DEFINES */
#define NUM_BASE	10

/* CLASS */
// Command Handler Class
// Inherits from the InputHandler to process specific input of commands necessary
// for this particular program.
class CommandHandler :
	public InputHandler
{
public:
	static CommandHandler* getInstance(GLFWwindow *rWindow);
	static CommandHandler* getInstance(const char* c_Input, int iInputSize, GLFWwindow *rWindow);
	~CommandHandler();

	//void process_Input();
	void handleKeyBoardInput(int cKey, int iAction, int iMods);

private:
	// Singleton Variables
	CommandHandler(GLFWwindow *rWindow);
	CommandHandler(const char* c_Input, int iInputSize, GLFWwindow *rWindow) : InputHandler(c_Input, iInputSize) { m_pGPXMngr = GameManager::getInstance(rWindow); }
	static CommandHandler* m_pInstance;
	GameManager *m_pGPXMngr;
	EntityManager *m_pEntMngr;

	// internal variables
	bool bWireFrame;
};

