#include "stdafx.h"
#include "InputHandler.h"
#include "GameManager.h"
#include "EntityHeaders/HovercraftEntity.h"
#include "EntityManager.h"
#include "SoundManager.h"
#include "GameStats.h"
// debug
#include "UserInterface/GameInterface.h"

/***********\
 * Defines *
\***********/
#define ZOOM_SCALE 0.05f

// Single Singleton instance
InputHandler* InputHandler::m_pInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *rWindow)
{
    m_gameManager = GAME_MANAGER;
    m_pEntityManager = ENTITY_MANAGER;
    // Keyboard
    glfwSetKeyCallback(rWindow, InputHandler::keyCallback);
    // Mouse
    m_bRotateFlag = m_bTranslateFlag = false;
    glfwSetMouseButtonCallback(rWindow, InputHandler::mouseButtonCallback);
    glfwSetCursorPosCallback(rWindow, InputHandler::mouseMoveCallback);
    glfwSetScrollCallback(rWindow, InputHandler::mouseScrollCallback);
    // Controller
    initializeJoysticksAtStart();
    glfwSetJoystickCallback(InputHandler::joystickCallback);
}

/*
@return singleton instance
*/
InputHandler* InputHandler::getInstance(GLFWwindow *rWindow)
{
    if (nullptr == m_pInstance)
        m_pInstance = new InputHandler(rWindow);

    return m_pInstance;
}

InputHandler* InputHandler::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

InputHandler::~InputHandler()
{
}


/*
Receives input from keyborad and updates the key status in pressed. Since keys
are updated through call backs, which may be faster than every frame update,
we may have multiple key callbacks per frame update. This means, we cannot reliably
check for just pressed or just released key statuses here.
NOTE: Keyboard input is read from CommandHandler. All keyboard input processing
should be done in the CommandHandler, not here.
*/
void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    /*
    Reject unknown keys. We only want to process keys available to standard keyboards.
    It is fastest to exit early while we can.
    */
    if (GLFW_KEY_UNKNOWN == key)
        return;

    /*
    Possible actions:
           GLFW_RELEASE = 0
           GLFW_PRESS   = 1
           GLFW_REPEAT  = 2

    We can save time by ignoring all GLFW_REPEAT callbacks, as these can be set
    during input processing.
    */
    switch (action)
    {
    case GLFW_PRESS:
        /*
        Since key callbacks occur at a faster rate than frame updates, both
        initial key presses and repeated key pressed will be viewed as just
        pressed until the input is processed for that fram. Only after it is
        processed, it can be changed to INPUT_PRESSED if the key remains
        pressed.

        We want to make sure that if the key is already pressed and is repeated
        (ie. the state is INPUT_PRESSED), that we do not override it with
        INPUT_JUST_PRESSED.
        */
        m_pInstance->m_keys[key] = INPUT_JUST_PRESSED;
        break;
    case GLFW_RELEASE:
        /*
        As the key has just be released, we keep the key in the map, but note
        it as just released. This allows us to parse for "just released"
        commands, such as returning to the default camera, after holding the
        camera swtich key. Once this key has been read has just released, it
        will be removed from the map.
        */
        m_pInstance->m_keys[key] = INPUT_JUST_RELEASED;
        break;
    }

#ifndef NDEBUG
    m_pInstance->debugKeyCommands(key, action);
#endif
}

#ifndef NDEBUG
/*
After being introduced and then removed, debugKeyCommands has returned from the grave.
I guess if you really want to do your debug key commands here, you can. Have fun!
*/
void InputHandler::debugKeyCommands(int key, int action)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            // Testing sound
        case GLFW_KEY_5:
            m_pEntityManager->getPlayer(HOVERCRAFT_PLAYER_1)->enablePowerup(POWERUP_SPEED_BOOST);
            break;
        case GLFW_KEY_6:
            m_pEntityManager->getPlayer(HOVERCRAFT_PLAYER_1)->enablePowerup(POWERUP_ROCKET_COOLDOWN);
            break;
        case GLFW_KEY_7:
            m_pEntityManager->getPlayer(HOVERCRAFT_PLAYER_1)->enablePowerup(POWERUP_SPIKES_COOLDOWN);
            break;
        case GLFW_KEY_8:
            m_pEntityManager->getPlayer(HOVERCRAFT_PLAYER_1)->enablePowerup(POWERUP_DASH_RECHARGE);
            break;
        case GLFW_KEY_9:
            GameInterface::getInstance(GAME_MANAGER->m_iWidth, GAME_MANAGER->m_iHeight)->setFocus(HOVERCRAFT_PLAYER_2);
            break;
        case GLFW_KEY_0:
            GameInterface::getInstance(GAME_MANAGER->m_iWidth, GAME_MANAGER->m_iHeight)->setFocus(HOVERCRAFT_PLAYER_1);
            break;
        case GLFW_KEY_MINUS:
            // SOUND_MANAGER->play(SoundManager::MUSIC_INGAME);
            break;
        case GLFW_KEY_DOWN:
            // SOUND_MANAGER->play(SoundManager::SOUND_HOVERCAR_ENGINE);
            break;
        case GLFW_KEY_LEFT:
            //SOUND_MANAGER->increaseSpeedParameter();
            // SOUND_MANAGER->upPosition();
            break;
        case GLFW_KEY_RIGHT:
            //SOUND_MANAGER->decreaseSpeedParameter();
            // SOUND_MANAGER->downPosition();
            break;
            // Testing game stats kill updates
        case GLFW_KEY_R:
            cout << "R" << endl;
            GAME_STATS->addScore(eHovercraft::HOVERCRAFT_PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_2);
            break;
        case GLFW_KEY_T:
            cout << "T" << endl;
            GAME_STATS->addScore(eHovercraft::HOVERCRAFT_PLAYER_2, GameStats::eAddScoreReason::HIT_PLAYER_1);
            break;
        case GLFW_KEY_Y:
            m_pEntityManager->getPlayer(HOVERCRAFT_PLAYER_1)->setLoseControl(1.0f);
            break;
        }
    }
}
#endif

/*
Mouse Button Callback
Handle mouse movement controls.
*/
void InputHandler::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // Left Click
    if (GLFW_MOUSE_BUTTON_1 == button)
    {
        // Get current Cursor Position.
        if (GLFW_PRESS == action)           // Start tracking for Left Mouse click
        {
            m_pInstance->mouseTStart();

#ifdef _DEBUG   // Intersect Plane for some Testing
            // Get cursor position
            double fX, fY;
            glfwGetCursorPos(window, &fX, &fY);
            m_pInstance->m_gameManager->intersectPlane(static_cast<float>(fX), static_cast<float>(fY)); // TESTING
#endif
        }
        else if (GLFW_RELEASE == action)    // Release tracking
            m_pInstance->mouseTEnd();
    }
    else if (GLFW_MOUSE_BUTTON_2 == button) // Right Click
    {
        if (GLFW_PRESS == action)           // Start tracking for Right Mouse Click
            m_pInstance->mouseRStart();
        else if (GLFW_RELEASE == action)    // end tracking
            m_pInstance->mouseREnd();
    }
}

/*
 Handles input from Mouse Moves.
@param window   the mouse corresponds to
@param x        x-coordinate of the mouse in window coordinates
@param y        y-coordinate of the mouse in window coordinates
*/
void InputHandler::mouseMoveCallback(GLFWwindow* window, double x, double y)
{
    if (m_pInstance->m_bRotateFlag)
        m_pInstance->m_gameManager->rotateCamera(m_pInstance->m_pInitialPos - vec2((float) x, (float) y));

    // Set new current position
    m_pInstance->m_pInitialPos.x = (float) x;
    m_pInstance->m_pInitialPos.y = (float) y;
}

/*
Handle scroll wheel callbacks

@param window   the mouse corresponds to
@param xoffset  of the mouse wheel
@param yoffset  of the mouse wheel
*/
void InputHandler::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_pInstance->m_gameManager->zoomCamera((float) yoffset * ZOOM_SCALE);
}

/*
Initializes all joysticks at the start of the program. This is necessary as
joysticks that are already connected before the program starts do not initiate
glfwSetJoystickCallback(), which initializes controllers that are connected
mid-program.
*/
void InputHandler::initializeJoysticksAtStart()
{
    initializeJoystickVariables();
    checkForPresentJoysticks();

    // Initialize all connnected joysticks
    // joystickCallback only checks for joysticks that go from disconnected to
    // connected, and therefore does not check for joysticks that are already
    // connected at the start of the program.
    for (int joystickID = GLFW_JOYSTICK_1; joystickID < MAX_PLAYER_COUNT; joystickID++)
    {
        initializeJoystick(joystickID);
    }
}

/*
Initialize joystick variables before they are set
*/
void InputHandler::initializeJoystickVariables()
{
    for (int player = 0; player < MAX_PLAYER_COUNT; player++)
    {
        m_pJoystickIsPresent[player] = false;
        m_pJoystickAxesCount[player] = 0;
        m_pJoystickButtonCount[player] = 0;
        m_pJoystickNames[player] = EMPTY_CONTROLLER;

        for (int button = 0; button < MAX_BUTTON_COUNT; button++)
        {
            m_joystickButtons[player][button] = INPUT_RELEASED;
        }
    }
}

/*
Check all if all joysticks are connected
*/
void InputHandler::checkForPresentJoysticks()
{
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        m_pJoystickIsPresent[i] = glfwJoystickPresent(i);
    }
}


/*
Initializes a joysticks of a given joystickID. It will only initialize if the
joystick is actually present.

@param joystickID   to initialize
*/
void InputHandler::initializeJoystick(int joystickID)
{
    // Check if joystick is present. Can only initialize if present.
    m_pJoystickIsPresent[joystickID] = glfwJoystickPresent(joystickID);
    if (!m_pJoystickIsPresent[joystickID])
    {
        return;
    }
    // Axis states
    m_pJoystickAxes[joystickID] = glfwGetJoystickAxes(joystickID, &m_pJoystickAxesCount[joystickID]);

    // Button states
    m_pJoystickButtonsRaw[joystickID] = glfwGetJoystickButtons(joystickID, &m_pJoystickButtonCount[joystickID]);

    // Names
    m_pJoystickNames[joystickID] = glfwGetJoystickName(joystickID);

#ifdef NDEBUG
    // debugPrintJoystickInformation(joystickID);
#endif
}

#ifndef NDEBUG
// DEBUG Print information about all joysticks
void InputHandler::debugPrintJoystickInformation()
{
    system("CLS");
    for (int joystickID = 0; joystickID < MAX_PLAYER_COUNT; joystickID++)
    {
        debugPrintJoystickInformation(joystickID);
    }
}
#endif

#ifndef NDEBUG
// DEBUG Print information about a joystick
void InputHandler::debugPrintJoystickInformation(int joystickID)
{
    std::cout << m_pJoystickNames[joystickID] << " " << joystickID
        << " is " <<  (m_pJoystickIsPresent[joystickID] ? "" : "dis" ) << "connected"
        << std::endl;

    // debugPrintJoystickAxes(joystickID);
    // debugPrintJoystickButtons(joystickID);
}
#endif

#ifndef NDEBUG
void InputHandler::debugPrintJoystickAxes(int joystickID)
{
    if (!m_pJoystickIsPresent[joystickID])
    {
        return;
    }
    const float* axes = m_pJoystickAxes[joystickID];
    std::cout << "\tAxes[" << m_pJoystickAxesCount[joystickID] << "]: " << std::endl
              << "\t\tLeft stick x: "  << axes[AXIS_LEFT_STICK_X]  << std::endl
              << "\t\t           y: "  << axes[AXIS_LEFT_STICK_Y]  << std::endl
              << "\t\tRight stick x: " << axes[AXIS_RIGHT_STICK_X] << std::endl
              << "\t\t            y: " << axes[AXIS_RIGHT_STICK_Y] << std::endl
              << "\t\tLeft trigger: "  << axes[AXIS_LEFT_TRIGGER]  << std::endl
              << "\t\tRight trigger: " << axes[AXIS_RIGHT_TRIGGER] << std::endl;
    std::cout << "\t\t[";
    for (int i = 0; i < m_pJoystickAxesCount[joystickID]; i++)
    {
        std::cout << axes[i] << " ";
    }
    std::cout << "]" << std::endl;
}
#endif

#ifndef NDEBUG
void InputHandler::debugPrintJoystickButtons(int joystickID)
{
    if (!m_pJoystickIsPresent[joystickID])
    {
        return;
    }
    const unsigned char* buttonsPressed = m_pJoystickButtonsRaw[joystickID];
    std::cout << "\tButtons[" << m_pJoystickButtonCount[joystickID] << "]: " << std::endl
              << "\t\tA: "            << buttonsPressed[BUTTON_A]            << std::endl
              << "\t\tB: "            << buttonsPressed[BUTTON_B]            << std::endl
              << "\t\tX: "            << buttonsPressed[BUTTON_X]            << std::endl
              << "\t\tY: "            << buttonsPressed[BUTTON_Y]            << std::endl
              << "\t\tLeft Bumper: "  << buttonsPressed[BUTTON_LEFT_BUMPER]  << std::endl
              << "\t\tRight Bumper: " << buttonsPressed[BUTTON_RIGHT_BUMPER] << std::endl
              << "\t\tBack: "         << buttonsPressed[BUTTON_BACK]         << std::endl
              << "\t\tStart: "        << buttonsPressed[BUTTON_START]        << std::endl
              << "\t\tLeft Stick: "   << buttonsPressed[BUTTON_LEFT_STICK]   << std::endl
              << "\t\tRight Stick: "  << buttonsPressed[BUTTON_RIGHT_STICK]  << std::endl
              << "\t\tUp: "           << buttonsPressed[BUTTON_UP]           << std::endl
              << "\t\tRight: "        << buttonsPressed[BUTTON_RIGHT]        << std::endl
              << "\t\tDown: "         << buttonsPressed[BUTTON_DOWN]         << std::endl
              << "\t\tLeft: "         << buttonsPressed[BUTTON_LEFT]         << std::endl;
    std::cout << "\t\t[";
    for (int i = 0; i < m_pJoystickButtonCount[joystickID]; i++)
    {
        std::cout << buttonsPressed[i] << " ";
    }
    std::cout << "]" << std::endl;
}
#endif

/*
Registers a joystick as not connected to the game.
*/
void InputHandler::disconnectJoystick(int joystickID)
{
    m_pJoystickIsPresent[joystickID] = false;
    // debugPrintJoystickInformation(joystickID);

}

/*
Checks for joystick connection/disconnection.

@param joystickID   to check
@param event        to indicate what happened to joystick
*/
void InputHandler::joystickCallback(int joystickID, int event)
{
    if (event == GLFW_CONNECTED)
    {
        m_pInstance->initializeJoystick(joystickID);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        m_pInstance->disconnectJoystick(joystickID);
    }
}

/*
Get the input status of all the present controllers and stores it.
This is called once per frame.
*/
void InputHandler::updateJoysticks()
{
    for (int joystickID = GLFW_JOYSTICK_1; joystickID < MAX_PLAYER_COUNT; joystickID++)
    {
        if (m_pJoystickIsPresent[joystickID])
        {
            // Current axis states
            m_pJoystickAxes[joystickID] = glfwGetJoystickAxes(joystickID, &m_pJoystickAxesCount[joystickID]);
            // Current button states
            m_pJoystickButtonsRaw[joystickID] = glfwGetJoystickButtons(joystickID, &m_pJoystickButtonCount[joystickID]);

            // Final button states
            updateJoystickButtonStates(joystickID);
        }
    }
}

int InputHandler::getJoystickCount()
{
    int count = 0;
    for (int joystick : m_pJoystickIsPresent)
    {
        count += joystick;
    }
    return count;
}

/*
Convert raw joystick input values to values that the command handler can read and
alter after processing.

Can only set values to just pressed and just released if there is a change in
pressed/released state. Changes to pressed and released are made in the
CommandHandler after the input has been processed.

@param joystickID   to update
*/
void InputHandler::updateJoystickButtonStates(int joystickID)
{
    // Update actual buttons
    for (int button = BUTTON_A; button < MAX_BUTTON_INDEX; button++)
    {
        if (m_pJoystickButtonsRaw[joystickID][button] == GLFW_PRESS
            && m_joystickButtons[joystickID][button] != INPUT_PRESSED)
        {
            m_joystickButtons[joystickID][button] = INPUT_JUST_PRESSED;
        }
        else if (m_pJoystickButtonsRaw[joystickID][button] == GLFW_RELEASE
            && m_joystickButtons[joystickID][button] != INPUT_RELEASED)
        {
            m_joystickButtons[joystickID][button] = INPUT_JUST_RELEASED;
        }
    }
    // Update triggers as buttons
    if (m_pJoystickAxes[joystickID][AXIS_LEFT_TRIGGER] > TRIGGER_IS_NETURAL
        && m_joystickButtons[joystickID][TRIGGER_LEFT] != INPUT_PRESSED)
    {
        m_joystickButtons[joystickID][TRIGGER_LEFT] = INPUT_JUST_PRESSED;
    }
    else if (m_pJoystickAxes[joystickID][AXIS_LEFT_TRIGGER] == TRIGGER_IS_NETURAL
        && m_joystickButtons[joystickID][TRIGGER_LEFT] != INPUT_RELEASED)
    {
        m_joystickButtons[joystickID][TRIGGER_LEFT] = INPUT_JUST_RELEASED;
    }

    if (m_pJoystickAxes[joystickID][AXIS_RIGHT_TRIGGER] > TRIGGER_IS_NETURAL
        && m_joystickButtons[joystickID][TRIGGER_RIGHT] != INPUT_PRESSED)
    {
        m_joystickButtons[joystickID][TRIGGER_RIGHT] = INPUT_JUST_PRESSED;
    }
    else if (m_pJoystickAxes[joystickID][AXIS_RIGHT_TRIGGER] == TRIGGER_IS_NETURAL
        && m_joystickButtons[joystickID][TRIGGER_RIGHT] != INPUT_RELEASED)
    {
        m_joystickButtons[joystickID][TRIGGER_RIGHT] = INPUT_JUST_RELEASED;
    }
}
