#pragma once

// Forward Declaration of Game Manager and Entity Manager
class GameManager;
class EntityManager;

/*
    Receives and stores user input (mouse, keyboard and controller).
*/
class InputHandler final
{
public:

    /*
        The states of joystick buttons. This is for expanding on joystick
        states, as GLFW only provides GLFW_PRESS and GLFW_RELEASE.
        Just pressed and just released states allow us to register certain
        commands only once if a key or button is pressed and held down instead
        of repeatedly.
    */
    enum eInputState
    {
        INPUT_RELEASED,
        INPUT_JUST_PRESSED,
        INPUT_PRESSED,
        INPUT_JUST_RELEASED,
    };

    static InputHandler* getInstance(GLFWwindow *rWindow);
    static InputHandler* getInstance();
    ~InputHandler();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void joystickCallback(int joystickID, int event);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseMoveCallback(GLFWwindow* window, double x, double y);
    static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    void updateJoysticks();


    // Flag the Mouse for Rotations or Translations (Cannot do both at the same time).
    void mouseTStart() { m_bTranslateFlag = !m_bRotateFlag; }
    void mouseTEnd() { m_bTranslateFlag = false; }
    void mouseRStart() { m_bRotateFlag = !m_bTranslateFlag; };
    void mouseREnd() { m_bRotateFlag = false; }

    int getJoystickCount();

    void debugAXIS(int joystickID);

    // Getters and Setters for Joystick information
    bool                isJoystickPresent(unsigned int iJoystick)   { return m_pJoystickData[iJoystick].bPresent; }
    const float*        getAxesPointer(unsigned int iJoystick)      { return m_pJoystickData[iJoystick].pAxes; }
    eInputState*        getInputStateArray(unsigned int iJoystick, int* iReturnButtonCount, int* iReturnButtonMask)
    {
        *iReturnButtonCount = m_pJoystickData[iJoystick].iButtonCount;
        *iReturnButtonMask = m_pJoystickData[iJoystick].iMask;
        return m_pJoystickData[iJoystick].eButtonState;
    }

    /*
        Keyboard

        Hold the states of pressed, just pressed, and just released keys.
        Valid input states:
            INPUT_PRESSED
            INPUT_JUST_PRESSED
            INPUT_JUST_RELEASED

        Keys that are INPUT_RELEASED will not be present in the map to save
        us from having to iterate through all the released keys every update.

        Key: glfw key values
        Value: state of the key
    */
    map<int, eInputState> m_keys;

    // Joysticks
    /*
    List of joysticks that are present (detected) by the game.
    Only present controllers are initialized
    true - present
    0 - not present
    */
    //bool m_pJoystickIsPresent[MAX_PLAYER_COUNT];
    ///*
    //The number of axes each joystick has. Ideally, every joystick has 4 axes.
    //*/
    //int m_pJoystickAxesCount[MAX_PLAYER_COUNT];
    ///*
    //The value of each axis for each player.
    //*/
    //const float* m_pJoystickAxes[MAX_PLAYER_COUNT];
    ///*
    //The number of buttons that each joystick has. Ideally, every joystick would have the
    //same number of buttons, but we can never be too sure.
    //*/
    //int m_pJoystickButtonCount[MAX_PLAYER_COUNT];
    ///*
    //The input states of each button for each joystick.
    //
    //Index retrieval
    //0 : int joystickID
    //1 : int button macro
    //*/
    //eInputState m_joystickButtons[MAX_PLAYER_COUNT][MAX_BUTTON_COUNT];
    ///*
    //The names of each joystick. This is only important for debugging purposes.
    //*/
    //const char* m_pJoystickNames[MAX_PLAYER_COUNT];
private:
    // Singleton Variables
    InputHandler(GLFWwindow *rWindow);
    void checkForPresentJoysticks();

    // Joystick information
    struct sJoystickInfo
    {
        /*
            List of joysticks that are present (detected) by the game.
            Only present controllers are initialized
            true - present
            0 - not present
        */
        bool            bPresent;
        /*
            Mask for referencing controls for different joystick types.
        */
        int             iMask;
        /*
            The number of axes each joystick has. Ideally, every joystick has 4 axes.
            0 Left stick x-direction
            1 Left stick y-direction
            2 Right stick x-direction
            3 Right stick y-direction
        */
        int             iAxesCount;
        /*
            The value of each axis for each player.
        */
        const float*    pAxes;
        /*
            The names of each joystick. This is only important for debugging
            purposes.
        */
        string          sName;
        /*
            The number of buttons that each joystick has. Ideally, every
            joystick would have the same number of buttons, but we can never be
            too sure.
        */
        int             iButtonCount;
        /*
            The input states of each button for each joystick.
            
            Index retrieval:
            The XBOX_BUTTON and PS4_BUTTON macros define which index
            corresponds which which button. These values are empirically
            determined. The introduction of a new controller type therefore
            cannot guarantee to follow the same button indices.
        */
        eInputState     eButtonState[MAX_BUTTON_COUNT];
        /*
            Raw input values retrieved from glfwGetJoystickButtons()
            The values are either GLFW_PRESS or GLFW_RELEASE
        */
        const unsigned char* pRawButtons;

        /*
            Initializes Default Values for a joystick
        */
        void initialize()
        {
            // Set Default Values
            bPresent        = false;
            iMask           = XBOX_MASK;
            iAxesCount      = 0;
            pAxes           = nullptr;
            iButtonCount    = 0;
            sName           = EMPTY_CONTROLLER;
            pRawButtons     = nullptr;

            // Set all Button States to Released
            for (int button = 0; button < MAX_BUTTON_COUNT; button++)
                eButtonState[button] = INPUT_RELEASED;
        }
    } m_pJoystickData[MAX_PLAYER_COUNT];

#ifndef NDEBUG
    void debugKeyCommands(int key, int action);
    void debugPrintJoystickAxes(int joystickID);
    void debugPrintJoystickButtons(int joystickID);
    void debugPrintJoystickInformation();
    void debugPrintJoystickInformation(int joystickID);
#endif
    void disconnectJoystick(int joystickID);
    void initializeJoystick(int joystickID);
    void initializeJoystickVariables();
    void initializeJoysticksAtStart();

    static InputHandler* m_pInstance;

    // Mouse
    bool m_bTranslateFlag;
    bool m_bRotateFlag;
    glm::vec2 m_pInitialPos;

    // The only reason this needed is for mouse input, which may not actually
    // be needed for the final product. ie. this may be removed later on.
    GameManager*    m_pGameManager;
    EntityManager*  m_pEntityManager;

    // Joysticks
    void updateJoystickButtonStates(int joystick);
};
