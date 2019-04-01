#include "Menus/GameMenu.h"
#include "Menus/PauseMenu.h"
#include "CommandHandler.h"
#include "EntityHeaders/HovercraftEntity.h"
#include "UserInterface/PauseInterface.h"

// Singleton instance
GameMenu* GameMenu::m_pInstance = nullptr;
/*
Constructor
*/
void GameMenu::enter()
{
    m_pGameManager->setPaused(false);
}
GameMenu::GameMenu() : Menu(
    // m_pressedKeyToFixedCommand
    unordered_map<int, eFixedCommand>
    {
        {GLFW_KEY_TAB,          COMMAND_MENU_BACK},
        {GLFW_KEY_ENTER,        COMMAND_MENU_START},
        {GLFW_KEY_W,            COMMAND_MOVE_FORWARD},
        {GLFW_KEY_A,            COMMAND_MOVE_LEFT},
        {GLFW_KEY_S,            COMMAND_MOVE_BACK},
        {GLFW_KEY_D,            COMMAND_MOVE_RIGHT},
        {GLFW_KEY_J,            COMMAND_TURN_LEFT},
        {GLFW_KEY_L,            COMMAND_TURN_RIGHT},
        },
    // justPressedKeyToFixedCommand
    unordered_map<int, eFixedCommand>
    {
        {GLFW_KEY_P,            COMMAND_MENU_PAUSE_TOGGLE},
        {GLFW_KEY_LEFT_SHIFT,   COMMAND_ABILITY_TRAIL_ACTIVATE},
        {GLFW_KEY_SPACE,        COMMAND_ABILITY_ROCKET},
        {GLFW_KEY_LEFT_CONTROL, COMMAND_ABILITY_SPIKES},
        {GLFW_KEY_K,            COMMAND_DASH_BACK},
        {GLFW_KEY_I,            COMMAND_DASH_FORWARD},
        {GLFW_KEY_H,            COMMAND_DASH_LEFT},
        {GLFW_KEY_SEMICOLON,    COMMAND_DASH_RIGHT},
        //
        {GLFW_KEY_ESCAPE,       COMMAND_CLOSE_WINDOW},
        {GLFW_KEY_TAB,          COMMAND_MENU_BACK},
        {GLFW_KEY_COMMA,        COMMAND_CAMERA_BACK},
#ifndef NDEBUG
        {GLFW_KEY_C,            COMMAND_DEBUG_TOGGLE_DEBUG_CAMERA},
        {GLFW_KEY_F,            COMMAND_DEBUG_TOGGLE_WIREFRAME},
        {GLFW_KEY_1,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER1},
        {GLFW_KEY_2,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER2},
        {GLFW_KEY_3,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER3},
        {GLFW_KEY_4,            COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER4},
        {GLFW_KEY_B,            COMMAND_DEBUG_TOGGLE_DRAW_BOUNDING_BOXES},
        {GLFW_KEY_M,            COMMAND_DEBUG_TOGGLE_DRAW_SPATIAL_MAP},
        {GLFW_KEY_KP_0,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_0},
        {GLFW_KEY_KP_1,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_1},
        {GLFW_KEY_KP_2,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_2},
        {GLFW_KEY_KP_3,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_3},
        {GLFW_KEY_KP_4,         COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_4},
#endif
    },
    // justReleasedKeyToFixedCommand
    unordered_map<int, eFixedCommand>
    {
        {GLFW_KEY_LEFT_SHIFT,   COMMAND_ABILITY_TRAIL_DEACTIVATE},
        {GLFW_KEY_COMMA,        COMMAND_CAMERA_FRONT},
    },
    // repeatButtonToFixedCommand
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_BACK,            COMMAND_MENU_BACK},
        {BUTTON_LEFT_STICK,      COMMAND_INVALID_FIXED },
        {BUTTON_RIGHT_STICK,     COMMAND_INVALID_FIXED },
        {BUTTON_UP,              COMMAND_INVALID_FIXED },
        {BUTTON_RIGHT,           COMMAND_INVALID_FIXED },
        {BUTTON_DOWN,            COMMAND_INVALID_FIXED },
        {BUTTON_LEFT,            COMMAND_INVALID_FIXED },
        {MAX_BUTTON_INDEX,       COMMAND_INVALID_FIXED },
        {BUTTON_START,           COMMAND_INVALID_FIXED },
        },
    // justPressedButtonToFixedCommand
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_LEFT_BUMPER,    COMMAND_ABILITY_SPIKES},
        {BUTTON_A,              COMMAND_DASH_BACK},
        {BUTTON_Y,              COMMAND_DASH_FORWARD},
        {BUTTON_X,              COMMAND_DASH_LEFT},
        {BUTTON_B,              COMMAND_DASH_RIGHT},
        {BUTTON_RIGHT_BUMPER,   COMMAND_CAMERA_BACK},
        {BUTTON_START,          COMMAND_MENU_PAUSE_TOGGLE},
        {BUTTON_BACK,           COMMAND_MENU_START}, // TODO for now until we find a purpose
        {BUTTON_UP,             COMMAND_HONK_UP},
        {BUTTON_RIGHT,          COMMAND_HONK_RIGHT},
        {BUTTON_DOWN,           COMMAND_HONK_DOWN},
        {BUTTON_LEFT,           COMMAND_HONK_LEFT},
        {TRIGGER_LEFT,          COMMAND_ABILITY_TRAIL_ACTIVATE},
        {TRIGGER_RIGHT,         COMMAND_ABILITY_ROCKET},
        },
    // justReleasedButtonToFixedCommand
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_RIGHT_BUMPER,   COMMAND_CAMERA_FRONT},
        {TRIGGER_LEFT,          COMMAND_ABILITY_TRAIL_DEACTIVATE},
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

/*
    Get Singleton instance
*/
Menu* GameMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new GameMenu();
    }
    return m_pInstance;
}

/*
    Destructor
*/
GameMenu::~GameMenu()
{
}

/*
    Make a player execute a eFixedCommand.
    FixedCommands are binary in that either they are executed or they are not,
    with no extra parameters.

    For example: if a player PLAYER_2 executes the ABILITY_ROCKET command, that
    is all the information the program needs to know for that hovercraft to
    execute that command.

    @param player   to execute command on
    @param command  to execute
*/
void GameMenu::executeIfHovercraftExists(eHovercraft hovercraft,
                                         eFixedCommand command)
{
    if (m_pEntityMngr->playerExists(hovercraft))
    {
        HovercraftEntity* playerEntity = m_pEntityMngr->getPlayer(hovercraft);
        executeValidHovercraft(playerEntity, command);
    }
}

/*
    Make a player execute a fixed command.

    @param player       to execute command on
    @param command      to execute
*/
void GameMenu::executeValidHovercraft(HovercraftEntity *hovercraft,
                                      eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_ABILITY_ROCKET:
    case COMMAND_ABILITY_SPIKES:
    case COMMAND_ABILITY_TRAIL_ACTIVATE:
    case COMMAND_ABILITY_TRAIL_DEACTIVATE:
    case COMMAND_DASH_BACK:
    case COMMAND_DASH_FORWARD:
    case COMMAND_DASH_LEFT:
    case COMMAND_DASH_RIGHT:
        hovercraft->useAbility(m_fixedCommandToAbility.at(command));
        break;
    case COMMAND_CAMERA_FRONT:
        hovercraft->setActiveCameraToFront();
        break;
    case COMMAND_CAMERA_BACK:
        hovercraft->setActiveCameraToBack();
        break;
    case COMMAND_MENU_BACK:
       break;
    case COMMAND_MENU_PAUSE_TOGGLE:
        m_pGameManager->setCurrentInterface(PauseInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(PauseMenu::getInstance());
       break;
    case COMMAND_MENU_START:
       break;

    // Since the honk sound is played directly and all players hear the same
    // thing, we don't actually need to relate anything to the hovercraft
    // itself.
    case COMMAND_HONK_UP:
        SOUND_MANAGER->play(SoundManager::SOUND_HONK_UP);
        break;
    case COMMAND_HONK_RIGHT:
        SOUND_MANAGER->play(SoundManager::SOUND_HONK_RIGHT);
        break;
    case COMMAND_HONK_DOWN:
        SOUND_MANAGER->play(SoundManager::SOUND_HONK_DOWN);
        break;
    case COMMAND_HONK_LEFT:
        SOUND_MANAGER->play(SoundManager::SOUND_HONK_LEFT);
        break;
#ifndef NDEBUG
    // Can only close the game directly in game in debug mode, for convenience
    //case COMMAND_CLOSE_WINDOW:
        //glfwSetWindowShouldClose(COMMAND_HANDLER->m_pWindow, GL_TRUE);
        //break;

    case COMMAND_DEBUG_TOGGLE_WIREFRAME:
        debugToggleWireframe();
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER1:
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER2:
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER3:
        break;
    case COMMAND_DEBUG_SWITCH_KEYBOARD_TO_PLAYER4:
        break;
    case COMMAND_DEBUG_TOGGLE_DEBUG_CAMERA:
        m_pEntityMngr->toggleDebugCamera();
        break;
    case COMMAND_DEBUG_TOGGLE_DRAW_BOUNDING_BOXES:
        m_pEntityMngr->toggleBBDrawing();
        break;
    case COMMAND_DEBUG_TOGGLE_DRAW_SPATIAL_MAP:
        m_pEntityMngr->toggleSpatialMapDrawing();
        break;
//    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_0:
//        m_pGameManager->m_pCurrentInterface->setDisplayCount(0);
//        break;
//    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_1:
//        m_pGameManager->m_pCurrentInterface->setDisplayCount(1);
//        break;
//    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_2:
//        m_pGameManager->m_pCurrentInterface->setDisplayCount(2);
//        break;
//    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_3:
//        m_pGameManager->m_pCurrentInterface->setDisplayCount(3);
//        break;
//    case COMMAND_DEBUG_SET_UI_DISPLAY_COUNT_4:
//        m_pGameManager->m_pCurrentInterface->setDisplayCount(4);
//        break;
#endif
    }
}

/*
    Make a given player execute a eVariableCommand.
    VariableCommands require extra parameters to complete the command.
    Specifically, they require an axis or axes to make sense of the command.

    For example: if player PLAYER_1 executes the MOVE command, they also need
    to specify the x and y axis values to determine what direction and at what
    intensity to go at.

    Axes values are normalized and follow Cartesian coordinates:
                            y = 1
                              ^
                              |
                x = -1 <-- x, y = 0 --> x = 1
                              |
                              v
                            y = -1

    @param player   to execute command on
    @param command  to execute
    @param x        x-coordinate
    @param y        y-coordinate
*/
void GameMenu::executeIfHovercraftExists(eHovercraft hovercraft,
                                         eVariableCommand command,
                                         float x, float y)
{
    if ((command == COMMAND_INVALID_VARIABLE)
        || (!m_pEntityMngr->playerExists(hovercraft)))
    {
        return;
    }
    HovercraftEntity* playerEntity = m_pEntityMngr->getPlayer(hovercraft);
    executeValidHovercraft(playerEntity, command, x, y);
}

/*
    Make a player execute a variable command.

    @param player       to execute command on
    @param command      to execute
    @param x            x-coordinate
    @param y            y-coordinate
*/
void GameMenu::executeValidHovercraft(HovercraftEntity *hovercraft,
                                            eVariableCommand command,
                                            float x, float y)
{
    switch (command)
    {
    case COMMAND_MOVE:
        hovercraft->move(x, y);
        break;
    case COMMAND_TURN:
        hovercraft->turn(x);
        break;
    }
}

void GameMenu::setupKeyCommands()
{
    bMovementNeutral = true;
    bTurnNeutral = true;
    xMove = 0.0f;
    yMove = 0.0f;
    xTurn = 0.0f;
    yTurn = 0.0f;
}

void GameMenu::executeFixedCommand(eHovercraft hovercraft, eFixedCommand command)
{
    executeIfHovercraftExists(hovercraft, command);
}

/*
    A specified movement eFixedCommand may be called multiple times per frame.
    Since some of these movements commands are contradictory, we must
    accumulate them all to form a single movement command similar to that of
    joystick movement.

    @param command  for movement from keyboard
*/
void GameMenu::handleAccumulatedKeyCommands(eFixedCommand command)
{
    // Check for cummulative movement commands, which are only executed once
    // all movement keys are checked.
    switch (command)
    {
    case COMMAND_MOVE_FORWARD:
        bMovementNeutral = false;
        yMove += JOYSTICK_IS_MAX;
        break;
    case COMMAND_MOVE_BACK:
        bMovementNeutral = false;
        yMove += JOYSTICK_IS_MIN;
        break;
    case COMMAND_MOVE_LEFT:
        bMovementNeutral = false;
        xMove += JOYSTICK_IS_MIN;
        break;
    case COMMAND_MOVE_RIGHT:
        bMovementNeutral = false;
        xMove += JOYSTICK_IS_MAX;
        break;
    case COMMAND_TURN_LEFT:
        bTurnNeutral = false;
        xTurn += JOYSTICK_IS_MIN;
        break;
    case COMMAND_TURN_RIGHT:
        bTurnNeutral = false;
        xTurn += JOYSTICK_IS_MAX;
        break;
    }
}

/*
    Make a specified hovercraft execute accumulated key commands. This should
    be the keyboard player's hovercraft.

    @param hovercraft   to execute accumulated key commands
*/
void GameMenu::executeAccumulatedKeyCommands(eHovercraft hovercraft)
{
    // This is where keys are handled, it's assumed that xMove and yMove will
    // be binary on/off. Let's use this assumption to our advantage and we can
    // simply if them to the proper size instead of doing a sqrt calculation.
    if ((xMove != 0.0f) && (yMove != 0.0f))
    {
        xMove *= 0.7071f;       // TODO: What is this Magic Number?
        yMove *= 0.7071f;       // TODO: What is this Magic Number?
    }

    if (!bMovementNeutral)
    {
        updateLeftStick(hovercraft, xMove, yMove);
    }
    if (!bTurnNeutral)
    {
        updateRightStick(hovercraft, xTurn, yTurn);
    }
}

void GameMenu::updateLeftStick(eHovercraft hovercraft, float x, float y)
{
    return executeIfHovercraftExists(hovercraft, COMMAND_MOVE, x, y);
}

void GameMenu::updateRightStick(eHovercraft hovercraft, float x, float y)
{
    executeIfHovercraftExists(hovercraft, COMMAND_TURN, x, y);
}
