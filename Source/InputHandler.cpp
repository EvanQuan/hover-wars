#include "stdafx.h"
#include "InputHandler.h"
#include "GameManager.h"
#include "EntityHeaders/HovercraftEntity.h"
#include "EntityManager.h"
#include "UserInterface/PromptInterface.h"
#include "SoundManager.h"
#include "UserInterface/GameInterface.h"
#include "GameStats.h"

/***********\
 * Defines *
\***********/
#define ZOOM_SCALE 0.05f

// Single Singleton instance
InputHandler* InputHandler::m_pInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *rWindow)
{
    m_pGameManager = GAME_MANAGER;
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
    Receives input from keyborad and updates the key status in pressed. Since
    keys are updated through call backs, which may be faster than every frame
    update, we may have multiple key callbacks per frame update. This means, we
    cannot reliably check for just pressed or just released key statuses here.
    NOTE: Keyboard input is read from MenuManager. All keyboard input
    processing should be done in the MenuManager, not here.
*/
void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    /*
        Reject unknown keys. We only want to process keys available to standard
        keyboards. It is fastest to exit early while we can.
    */
    if (GLFW_KEY_UNKNOWN == key)
        return;

    /*
        Possible actions:
               GLFW_RELEASE = 0
               GLFW_PRESS   = 1
               GLFW_REPEAT  = 2

        We can save time by ignoring all GLFW_REPEAT callbacks, as these can be
        set during input processing.
    */
    switch (action)
    {
    case GLFW_PRESS:
        /*
            Since key callbacks occur at a faster rate than frame updates, both
            initial key presses and repeated key pressed will be viewed as just
            pressed until the input is processed for that fram. Only after it
            is processed, it can be changed to INPUT_PRESSED if the key remains
            pressed.

            We want to make sure that if the key is already pressed and is
            repeated (ie. the state is INPUT_PRESSED), that we do not override
            it with INPUT_JUST_PRESSED.
        */
        m_pInstance->m_keys[key] = INPUT_JUST_PRESSED;
        break;
    case GLFW_RELEASE:
        /*
            As the key has just be released, we keep the key in the map, but
            note it as just released. This allows us to parse for "just
            released" commands, such as returning to the default camera, after
            holding the camera swtich key. Once this key has been read has just
            released, it will be removed from the map.
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
    After being introduced and then removed, debugKeyCommands has returned from
    the grave. I guess if you really want to do your debug key commands here,
    you can. Have fun!
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
            GameInterface::getInstance(m_pGameManager->getWidth(), m_pGameManager->getHeight())->setFocus(HOVERCRAFT_PLAYER_2);
            break;
        case GLFW_KEY_0:
            GameInterface::getInstance(m_pGameManager->getWidth(), m_pGameManager->getHeight())->setFocus(HOVERCRAFT_PLAYER_1);
            break;
        case GLFW_KEY_MINUS:
            // SOUND_MANAGER->play(SoundManager::SOUND_MUSIC_INGAME);
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
            //cout << "R" << endl;
            GAME_STATS->addScore(eHovercraft::HOVERCRAFT_PLAYER_1, GameStats::eAddScoreReason::HIT_PLAYER_2);
            break;
        case GLFW_KEY_T:
            //cout << "T" << endl;
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
            //m_pInstance->m_pGameManager->intersectPlane(static_cast<float>(fX), static_cast<float>(fY)); // TESTING
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
    if (m_pInstance->m_bRotateFlag) {
        m_pInstance->m_pGameManager->rotateCamera(m_pInstance->m_pInitialPos - vec2((float)x, (float)y));
    }

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
    m_pInstance->m_pGameManager->zoomCamera((float) yoffset * ZOOM_SCALE);
}

/*
    Initializes all joysticks at the start of the program. This is necessary as
    joysticks that are already connected before the program starts do not
    initiate glfwSetJoystickCallback(), which initializes controllers that are
    connected mid-program.
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
        m_pJoystickData[player].initialize();
}

/*
    Check all if all joysticks are connected
*/
void InputHandler::checkForPresentJoysticks()
{
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        m_pJoystickData[i].bPresent = glfwJoystickPresent(i);
    }
}


/*
    Initializes a joysticks of a given joystickID. It will only initialize if
    the joystick is actually present.

    @param joystickID   to initialize
*/
void InputHandler::initializeJoystick(int joystickID)
{
    // Check if joystick is present. Can only initialize if present.
    m_pJoystickData[joystickID].bPresent = glfwJoystickPresent(joystickID);
    if (!m_pJoystickData[joystickID].bPresent)
    {
        return;
    }
    // Axis states
    m_pJoystickData[joystickID].pAxes = glfwGetJoystickAxes(joystickID, &m_pJoystickData[joystickID].iAxesCount);

    // Button states
    m_pJoystickData[joystickID].pRawButtons = glfwGetJoystickButtons(joystickID, &m_pJoystickData[joystickID].iButtonCount);

    cout << "Button count: " << m_pJoystickData[joystickID].iButtonCount << endl;

    // Set the Mask based on the Button Count.
    switch (m_pJoystickData[joystickID].iButtonCount)
    {
    case PS4_BUTTON_COUNT:
        m_pJoystickData[joystickID].iMask = PS4_MASK;
        break;
    case XBOX_BUTTON_COUNT:
    default:
        m_pJoystickData[joystickID].iMask = XBOX_MASK;
        break;
    }

    // Names
    m_pJoystickData[joystickID].sName = glfwGetJoystickName(joystickID);

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
    std::cout << m_pJoystickData[joystickID].sName << " " << joystickID
        << " is " <<  (m_pJoystickData[joystickID].bPresent ? "" : "dis" ) << "connected"
        << std::endl;

    // debugPrintJoystickAxes(joystickID);
    // debugPrintJoystickButtons(joystickID);
}
#endif

#ifndef NDEBUG

void InputHandler::debugAXIS(int joystickID)
{
    system("CLS");
    debugPrintJoystickAxes(joystickID);
}
void InputHandler::debugPrintJoystickAxes(int joystickID)
{
    if (!m_pJoystickData[joystickID].bPresent)
    {
        return;
    }
    const float* axes = m_pJoystickData[joystickID].pAxes;
    //for (unsigned int i = 0; i < m_pJoystickData[joystickID].iAxesCount; ++i)
    //    cout << "Axis(" << to_string(i) << "): " << axes[i] << endl;
    switch (m_pJoystickData[joystickID].iButtonCount)
    {
    case XBOX_BUTTON_COUNT:
        std::cout << "\tAxes[" << m_pJoystickData[joystickID].iAxesCount << "]: " << std::endl
            << "\t\tLeft stick x: "     << axes[AXIS_LEFT_STICK_X] << std::endl
            << "\t\t           y: "     << axes[AXIS_LEFT_STICK_Y] << std::endl
            << "\t\tRight stick x: "    << axes[AXIS_RIGHT_STICK_X] << std::endl
            << "\t\t            y: "    << axes[AXIS_RIGHT_STICK_Y] << std::endl
            << "\t\tLeft trigger: "     << axes[AXIS_LEFT_TRIGGER] << std::endl
            << "\t\tRight trigger: "    << axes[AXIS_RIGHT_TRIGGER] << std::endl;
        break;
    case PS4_BUTTON_COUNT:
        std::cout << "\tAxes[" << m_pJoystickData[joystickID].iAxesCount << "]: " << std::endl
            << "\t\tLeft stick x: "     << axes[PS4_AXIS_LEFT_X] << std::endl
            << "\t\t           y: "     << axes[PS4_AXIS_LEFT_Y] << std::endl
            << "\t\tRight stick x: "    << axes[PS4_AXIS_RIGHT_X] << std::endl
            << "\t\t            y: "    << axes[PS4_AXIS_RIGHT_Y] << std::endl
            << "\t\tL2: "               << axes[PS4_AXIS_L2] << std::endl
            << "\t\tR2: "               << axes[PS4_AXIS_R2] << std::endl;
        break;
    }
    std::cout << "\t\t[";
    for (int i = 0; i < m_pJoystickData[joystickID].iAxesCount; i++)
    {
        std::cout << axes[i] << " ";
    }
    std::cout << "]" << std::endl;
}
#endif

#ifndef NDEBUG
void InputHandler::debugPrintJoystickButtons(int joystickID)
{
    if (!m_pJoystickData[joystickID].bPresent)
    {
        return;
    }
    const unsigned char* buttonsPressed = m_pJoystickData[joystickID].pRawButtons;
    switch (m_pJoystickData[joystickID].iButtonCount)
    {
    case XBOX_BUTTON_COUNT:
        std::cout << "\tXbox Buttons[" << m_pJoystickData[joystickID].iButtonCount << "]: " << std::endl
            << "\t\tA: "            << buttonsPressed[XBOX_BUTTON_A] << std::endl
            << "\t\tB: "            << buttonsPressed[XBOX_BUTTON_B] << std::endl
            << "\t\tX: "            << buttonsPressed[XBOX_BUTTON_X] << std::endl
            << "\t\tY: "            << buttonsPressed[XBOX_BUTTON_Y] << std::endl
            << "\t\tLeft Bumper: "  << buttonsPressed[XBOX_BUTTON_LEFT_BUMPER] << std::endl
            << "\t\tRight Bumper: " << buttonsPressed[XBOX_BUTTON_RIGHT_BUMPER] << std::endl
            << "\t\tBack: "         << buttonsPressed[XBOX_BUTTON_BACK] << std::endl
            << "\t\tStart: "        << buttonsPressed[XBOX_BUTTON_START] << std::endl
            << "\t\tLeft Stick: "   << buttonsPressed[XBOX_BUTTON_LEFT_STICK] << std::endl
            << "\t\tRight Stick: "  << buttonsPressed[XBOX_BUTTON_RIGHT_STICK] << std::endl
            << "\t\tUp: "           << buttonsPressed[XBOX_BUTTON_UP] << std::endl
            << "\t\tRight: "        << buttonsPressed[XBOX_BUTTON_RIGHT] << std::endl
            << "\t\tDown: "         << buttonsPressed[XBOX_BUTTON_DOWN] << std::endl
            << "\t\tLeft: "         << buttonsPressed[XBOX_BUTTON_LEFT] << std::endl;
        break;
    case PS4_BUTTON_COUNT:
        std::cout << "\tPS4 Buttons[" << m_pJoystickData[joystickID].iButtonCount << "]: " << std::endl
            << "\t\tX: "        << buttonsPressed[PS4_BUTTON_X] << std::endl
            << "\t\tO: "        << buttonsPressed[PS4_BUTTON_O] << std::endl
            << "\t\tSquare: "   << buttonsPressed[PS4_BUTTON_SQUARE] << std::endl
            << "\t\tTriangle: " << buttonsPressed[PS4_BUTTON_TRIANGLE] << std::endl
            << "\t\tL1: "       << buttonsPressed[PS4_BUTTON_L1] << std::endl
            << "\t\tL2: "       << buttonsPressed[PS4_BUTTON_L2] << std::endl
            << "\t\tL3: "       << buttonsPressed[PS4_BUTTON_L3] << std::endl
            << "\t\tR1: "       << buttonsPressed[PS4_BUTTON_R1] << std::endl
            << "\t\tR2: "       << buttonsPressed[PS4_BUTTON_R2] << std::endl
            << "\t\tR3: "       << buttonsPressed[PS4_BUTTON_R3] << std::endl
            << "\t\tUp: "       << buttonsPressed[PS4_BUTTON_UP] << std::endl
            << "\t\tRight: "    << buttonsPressed[PS4_BUTTON_RIGHT] << std::endl
            << "\t\tDown: "     << buttonsPressed[PS4_BUTTON_DOWN] << std::endl
            << "\t\tLeft: "     << buttonsPressed[PS4_BUTTON_LEFT] << std::endl
            << "\t\tOptions: "  << buttonsPressed[PS4_BUTTON_OPTIONS] << std::endl
            << "\t\tShare: "    << buttonsPressed[PS4_BUTTON_SHARE] << std::endl
            << "\t\tTrackpad: " << buttonsPressed[PS4_BUTTON_TRACKPAD] << std::endl
            << "\t\tPS: "       << buttonsPressed[PS4_BUTTON_PS] << std::endl;
        break;
    }

    std::cout << "\t\t[";
    for (int i = 0; i < m_pJoystickData[joystickID].iButtonCount; i++)
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
    m_pJoystickData[joystickID].initialize();
    // debugPrintJoystickInformation(joystickID);

}

/*
    Checks for joystick connection/disconnection.

    @param joystickID   to check
    @param event        to indicate what happened to joystick
*/
void InputHandler::joystickCallback(int joystickID, int event)
{
    string name;
    switch (event)
    {
    case GLFW_CONNECTED:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CONTROLLER_CONNECT);
        m_pInstance->initializeJoystick(joystickID);
        name = m_pInstance->m_pJoystickData[joystickID].sName;
        GAME_MANAGER->getMenuInterface()->displayGlobalMessage(name + " " + std::to_string(joystickID + 1) + " connected");
        break;
    case GLFW_DISCONNECTED:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CONTROLLER_CONNECT);
        m_pInstance->disconnectJoystick(joystickID);
        GAME_MANAGER->getMenuInterface()->displayGlobalMessage("Controller " + std::to_string(joystickID + 1) +  " disconnected.");
        break;
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
        if (m_pJoystickData[joystickID].bPresent)
        {
            // Current axis states
            m_pJoystickData[joystickID].pAxes = glfwGetJoystickAxes(joystickID, &m_pJoystickData[joystickID].iAxesCount);
            // Current button states
            m_pJoystickData[joystickID].pRawButtons = glfwGetJoystickButtons(joystickID, &m_pJoystickData[joystickID].iButtonCount);

            // Final button states
            updateJoystickButtonStates(joystickID);
        }
    }
}

int InputHandler::getJoystickCount()
{
    int count = 0;
    for (sJoystickInfo joystick : m_pJoystickData)
    {
        count += joystick.bPresent;
    }
    return count;
}

/*
    Convert raw joystick input values to values that the command handler can
    read and alter after processing.

    Can only set values to just pressed and just released if there is a change in
    pressed/released state. Changes to pressed and released are made in the
    MenuManager after the input has been processed.

    @param joystickID   to update
*/
void InputHandler::updateJoystickButtonStates(int joystickID)
{
    // Double check
    // Even though the controller is checked if it is present in the parent
    // updateJoysticks() call, without this redundant check, the game will crash
    // if a controller is disconnected mid-program.
    if (!m_pJoystickData[joystickID].bPresent)
    {
        return;
    }
    // Update actual buttons
    for (int button = 0; button < m_pJoystickData[joystickID].iButtonCount; button++)
    {
        if (m_pJoystickData[joystickID].pRawButtons[button] == GLFW_PRESS
            && m_pJoystickData[joystickID].eButtonState[button] != INPUT_PRESSED)
        {
            m_pJoystickData[joystickID].eButtonState[button] = INPUT_JUST_PRESSED;
        }
        else if (m_pJoystickData[joystickID].pRawButtons[button] == GLFW_RELEASE
            && m_pJoystickData[joystickID].eButtonState[button] != INPUT_RELEASED)
        {
            m_pJoystickData[joystickID].eButtonState[button] = INPUT_JUST_RELEASED;
        }
    }

    eInputState* pRightTriggerState, *pLeftTriggerState;
    const float* pLeftTriggerAxis, *pRightTriggerAxis;

    // Get Pointers to Triggers
    switch (m_pJoystickData[joystickID].iButtonCount)
    {
    case PS4_BUTTON_COUNT:
        pRightTriggerAxis = &m_pJoystickData[joystickID].pAxes[PS4_AXIS_R2];
        pRightTriggerState = &m_pJoystickData[joystickID].eButtonState[PS4_BUTTON_R2];
        pLeftTriggerAxis = &m_pJoystickData[joystickID].pAxes[PS4_AXIS_L2];
        pLeftTriggerState = &m_pJoystickData[joystickID].eButtonState[PS4_BUTTON_L2];
        break;
    case XBOX_BUTTON_COUNT:
    default:
        pRightTriggerAxis = &m_pJoystickData[joystickID].pAxes[AXIS_RIGHT_TRIGGER];
        pRightTriggerState = &m_pJoystickData[joystickID].eButtonState[XBOX_TRIGGER_RIGHT];
        pLeftTriggerAxis = &m_pJoystickData[joystickID].pAxes[AXIS_LEFT_TRIGGER];
        pLeftTriggerState = &m_pJoystickData[joystickID].eButtonState[XBOX_TRIGGER_LEFT];
        break;
    }

    // Update Triggers as buttons
    // Left Trigger
    if (*pLeftTriggerAxis > TRIGGER_IS_NETURAL
        && *pLeftTriggerState != INPUT_PRESSED)
    {
        *pLeftTriggerState = INPUT_JUST_PRESSED;
    }
    else if (*pLeftTriggerAxis == TRIGGER_IS_NETURAL
        && *pLeftTriggerState != INPUT_RELEASED)
    {
        *pLeftTriggerState = INPUT_JUST_RELEASED;
    }

    // Right Trigger
    if (*pRightTriggerAxis > TRIGGER_IS_NETURAL
        && *pRightTriggerState != INPUT_PRESSED)
    {
        *pRightTriggerState = INPUT_JUST_PRESSED;
    }
    else if (*pRightTriggerAxis == TRIGGER_IS_NETURAL
        && *pRightTriggerState != INPUT_RELEASED)
    {
        *pRightTriggerState = INPUT_JUST_RELEASED;
    }

    // system("CLS");
    // cout << "left trigger: " << *pLeftTriggerAxis << endl;
    // cout << "right trigger: " << *pRightTriggerAxis << endl;

}
