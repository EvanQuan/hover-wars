#pragma once

/* INCLUDES */
#include <map>
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"

/* DEFINES */
#define NUM_BASE	10

/* CLASS */
// Command Handler Class
class CommandHandler
{
public:
	static CommandHandler* getInstance(GLFWwindow *rWindow);
	~CommandHandler();

	static enum Command
	{
		ABILITY_ROCKET,
		ABILITY_SPIKES,
		ABILITY_TRAIL,
		CAMERA_FLIP,
		DASH_BACK,
		DASH_FORWARD,
		DASH_LEFT,
		DASH_RIGHT,
		MENU_PAUSE,
		MENU_SELECT,
		MENU_START,
		MOVE_BACK,
		MOVE_FORWARD,
		MOVE_LEFT,
		MOVE_RIGHT,
		TURN_LEFT,
		TURN_RIGHT,
		DEBUG_TOGGLE_WIREFRAME,
		NOTHING,
	};

	static enum PlayerNumber
	{
		PLAYER_ONE,
		PLAYER_TWO,
		PLAYER_THREE,
		PLAYER_FOUR,
	};

	void executeCommand(PlayerNumber player, Command command);

private:
	// Singleton Variables
	CommandHandler(GLFWwindow *rWindow);
	static CommandHandler* m_pInstance;
	GameManager *m_pGPXMngr;
	EntityManager *m_pEntMngr;

	// internal variables
	bool bWireFrame;

	// For input debugging
	std::map<Command, const char*> m_pCommandToString = {
		{ABILITY_ROCKET, "Rocket"},
		{ABILITY_SPIKES, "Spikes"},
		{ABILITY_TRAIL, "Trail"},
		{CAMERA_FLIP, "Flip"},
		{DASH_BACK, "Dash back"},
		{DASH_FORWARD, "Dash forward"},
		{DASH_LEFT, "Dash left"},
		{DASH_RIGHT, "Dash right"},
		{MENU_PAUSE, "Menu pause"},
		{MENU_SELECT, "Menu select"},
		{MENU_START, "Menu start"},
		{MOVE_BACK, "Move back"},
		{MOVE_FORWARD, "Move foward"},
		{MOVE_LEFT, "Move left"},
		{MOVE_RIGHT, "Move right"},
		{TURN_LEFT, "Turn left"},
		{TURN_RIGHT, "Turn right"},
		{DEBUG_TOGGLE_WIREFRAME, "Debug toggle wireframe"},
		{NOTHING, "Nothing"},
	};

	std::map<PlayerNumber, const char*> m_pPlayerNumberToString = {
		{PLAYER_ONE, "Player One"},
		{PLAYER_TWO, "Player Two"},
		{PLAYER_THREE, "Player Three"},
		{PLAYER_FOUR, "Player Four"},
	};
};

