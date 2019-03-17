#pragma once

#include "Menus/Menu.h"

class GameMenu final : Menu
{
public:
    ~GameMenu();

    static Menu* getInstance();

private:
    bool bMovementNeutral;
    bool bTurnNeutral;
    float xMove;
    float yMove;
    float xTurn;
    float yTurn;

    void enter();
protected:
    GameMenu();

    // Not time values
    void updateTimeValues(float fTimeInSeconds) {}

    static GameMenu* m_pInstance;

    void executeFixedCommand(eHovercraft hovercraft, eFixedCommand command);

    // For keyboard command handling
    void setupKeyCommands();
    void handleAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command);
    void executeAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command);

    // Joystick commands
    void updateLeftStick(eHovercraft hovercraft, float x, float y);
    void updateRightStick(eHovercraft hovercraft, float x, float y);


    void executeIfHovercraftExists(eHovercraft hovercraft, eFixedCommand command);
    void executeIfHovercraftExists(eHovercraft hovercraft, eVariableCommand command,
                                   float x, float y);

    void executeValidHovercraft(HovercraftEntity *hovercraft, eFixedCommand command);
    void executeValidHovercraft(HovercraftEntity *hovercraft,
                                eVariableCommand command, float x, float y);

    // For input debugging
    unordered_map<eFixedCommand, const char*> eFixedCommandToString =
    {
        {COMMAND_ABILITY_ROCKET,         "Rocket"},
        {COMMAND_ABILITY_SPIKES,         "Spikes"},
        {COMMAND_ABILITY_TRAIL_ACTIVATE, "Trail"},
        {COMMAND_CAMERA_BACK,            "Camera Back"},
        {COMMAND_CAMERA_FRONT,           "Camera Front"},
        {COMMAND_DASH_BACK,              "Dash back"},
        {COMMAND_DASH_FORWARD,           "Dash Forward"},
        {COMMAND_DASH_LEFT,              "Dash Left"},
        {COMMAND_DASH_RIGHT,             "Dash Right"},
        {COMMAND_MENU_BACK,              "Menu Back"},
        {COMMAND_MENU_PAUSE_TOGGLE,      "Menu Pause"},
        {COMMAND_MENU_START,             "Menu Start"},
        {COMMAND_INVALID_FIXED,          "Invalid"},
#ifndef NDEBUG

        {COMMAND_DEBUG_TOGGLE_WIREFRAME, "Debug Toggle Wireframe"},
#endif
    };
    // debugging
    unordered_map<eVariableCommand, const char*> eVariableCommandToString
    {
        {COMMAND_MOVE,             "Move"},
        {COMMAND_TURN,             "Turn"},
        {COMMAND_INVALID_VARIABLE, "Invalid"},
    };

    unordered_map<eFixedCommand, eAbility> m_fixedCommandToAbility =
    {
        {COMMAND_ABILITY_ROCKET,            ABILITY_ROCKET},
        {COMMAND_ABILITY_SPIKES,            ABILITY_SPIKES},
        {COMMAND_ABILITY_TRAIL_ACTIVATE,    ABILITY_TRAIL_ACTIVATE},
        {COMMAND_ABILITY_TRAIL_DEACTIVATE,  ABILITY_TRAIL_DEACTIVATE},
        {COMMAND_DASH_BACK,                 ABILITY_DASH_BACK},
        {COMMAND_DASH_FORWARD,              ABILITY_DASH_FORWARD},
        {COMMAND_DASH_LEFT,                 ABILITY_DASH_LEFT},
        {COMMAND_DASH_RIGHT,                ABILITY_DASH_RIGHT},
    };
};
