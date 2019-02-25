#pragma once

 #include "GameManager.h"
// class GameManager;
/*
Receives and stores user input (mouse, keyboard and controller).
*/
class InputHandler
{
public:

    /*
    The states of joystick buttons. This is for expanding on joystick states,
    as GLFW does not give this information.
    */
    enum eInputState
    {
        INPUT_RELEASED,
        INPUT_JUST_PRESSED,
        INPUT_PRESSED,
        INPUT_JUST_RELEASED,
    };

    static InputHandler* getInstance(GLFWwindow *rWindow);
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

    /*
    Keyboard

    Hold the states of pressed, just pressed, and just released keys.
    Valid input states:
        INPUT_PRESSED
        INPUT_JUST_PRESSED
        INPUT_JUST_RELEASED

    Keys that are INPUT_RELEASED will not be present in the map.

    Key: glfw key values
    Value: state of the key
    */
    map<int, eInputState> m_keys;

    // Joysticks
    /*
    List of joysticks that are present (detected) by the game.
    Only present controllers are initialized
    1 - present
    0 - not present
    */
    int m_pJoystickIsPresent[MAX_PLAYER_COUNT];
    /*
    The number of axes each joystick has. Ideally, every joystick has 4 axes.
    */
    int m_pJoystickAxesCount[MAX_PLAYER_COUNT];
    /*
    The value of each axis for each player.
    */
    const float* m_pJoystickAxes[MAX_PLAYER_COUNT];
    /*
    The number of buttons that each joystick has. Ideally, every joystick would have the
    same number of buttons, but we can never be too sure.
    */
    int m_pJoystickButtonCount[MAX_PLAYER_COUNT];
    /*
    The input states of each button for each joystick.

    Index retrieval
    0 : int joystickID
    1 : int button macro
    */
    eInputState m_joystickButtons[MAX_PLAYER_COUNT][MAX_BUTTON_COUNT];
    /*
    The names of each joystick. This is only important for debugging purposes.
    */
    const char* m_pJoystickNames[MAX_PLAYER_COUNT];
private:
    // Singleton Variables
    InputHandler(GLFWwindow *rWindow);
    void checkForPresentJoysticks();

#ifndef NDEBUG
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
    GameManager* m_gameManager;

    // Joysticks
    void updateJoystickButtonStates(int joystick);
    /*
    Raw input values retrieved from glfwGetJoystickButtons()
    The values are either GLFW_PRESS or GLFW_RELEASE
    */
    const unsigned char* m_pJoystickButtonsRaw[MAX_PLAYER_COUNT];
};
