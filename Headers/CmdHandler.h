#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "InputHandler.h"
#include "GraphicsManager.h"
#include "EnvironmentManager.h"

/* DEFINES */
#define CMD_SIZE	20
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

	void process_Input();
	void handleKeyBoardInput(int cKey, int iAction, int iMods);

private:
	// Singleton Variables
	CmdHandler(GLFWwindow *rWindow);
	CmdHandler(const char* c_Input, int iInputSize, GLFWwindow *rWindow) : InputHandler(c_Input, iInputSize) { m_pGPXMngr = GraphicsManager::getInstance(rWindow); }
	static CmdHandler* m_pInstance;
	GraphicsManager *m_pGPXMngr;
	EnvironmentManager *m_pEnvMngr;

	// internal variables
	bool bWireFrame;

	bool exec_SetB();
	bool exec_SetY();
	bool exec_SetAlpha();
	bool exec_SetBeta();
	bool exec_SetRGBVal(GraphicsManager::eRGB eRGBType);
	bool exec_SetColor();
	bool exec_Delete();
	bool exec_Load();
	bool exec_SetShine();
	bool exec_SetR();
	bool exec_SetThreshold();
	bool exec_SetThresholdMin();
	bool exec_SetThresholdMax();
	void outputHelpList();

	bool checkRange(float fVal, float fMIN, float fMAX);

	// Enum for Character Commands
	enum eCommands
	{
		HELP = 0,
		SET_B,
		SET_Y,
		SET_ALPHA,
		SET_BETA,
		SET_RC,
		SET_GC,
		SET_BC,
		SET_COLOR,
		LIST,
		DELETE,
		LOAD,
		TOGGLE_GOOCH,
		TOGGLE_X_TOON,
		TOGGLE_SPECULAR,
		SET_SHINE,
		SET_R,
		SET_THRESHOLD,
		SET_MIN_THRESHOLD,
		SET_MAX_THRESHOLD,
		NUM_CMDS
	};

	// All available commands
	static const char cCommands[NUM_CMDS][CMD_SIZE];
};

