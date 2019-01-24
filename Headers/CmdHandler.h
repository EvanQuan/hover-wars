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
class CmdHandler :
	public InputHandler
{
public:
	static CmdHandler* getInstance(GLFWwindow *rWindow);
	static CmdHandler* getInstance(const char* c_Input, int iInputSize, GLFWwindow *rWindow);
	~CmdHandler();

	//void process_Input();
	void handleKeyBoardInput(int cKey, int iAction, int iMods);

private:
	// Singleton Variables
	CmdHandler(GLFWwindow *rWindow);
	CmdHandler(const char* c_Input, int iInputSize, GLFWwindow *rWindow) : InputHandler(c_Input, iInputSize) { m_pGPXMngr = GameManager::getInstance(rWindow); }
	static CmdHandler* m_pInstance;
	GameManager *m_pGPXMngr;
	EntityManager *m_pEntMngr;

	// internal variables
	bool bWireFrame;
};

