#include "Menus/PromptMenu.h"
#include "CommandHandler.h"

/*
    If the user holds down a certain movement direction, there is a buffer time
    until the prompt movement repeats.

    Unit : seconds
*/
#define PROMPT_START_REPEAT_DELAY 1.0f
/*
    As the prompt repeats, there is a delay betweeen each repeat to ensure the
    cursor does not move too quickly.

    Unit : seconds
*/
#define PROMPT_REPEAT_DELAY 0.1f

PromptMenu::PromptMenu() : Menu(
    // pressedKey
    unordered_map<int, eFixedCommand>
    {
        // Movement
        // NOTE: potentially reuse movement commands
        {GLFW_KEY_W,     COMMAND_PROMPT_UP},
        {GLFW_KEY_UP,    COMMAND_PROMPT_UP},
        {GLFW_KEY_A,     COMMAND_PROMPT_LEFT},
        {GLFW_KEY_LEFT,  COMMAND_PROMPT_LEFT},
        {GLFW_KEY_S,     COMMAND_PROMPT_DOWN},
        {GLFW_KEY_DOWN,  COMMAND_PROMPT_DOWN},
        {GLFW_KEY_D,     COMMAND_PROMPT_RIGHT},
        {GLFW_KEY_RIGHT, COMMAND_PROMPT_RIGHT},
    },
    // justPressedKey
    unordered_map<int, eFixedCommand>
    {
        // Select
        {GLFW_KEY_SPACE,     COMMAND_PROMPT_SELECT},
        {GLFW_KEY_ENTER,     COMMAND_PROMPT_SELECT},
        {GLFW_KEY_BACKSPACE, COMMAND_PROMPT_BACK},
        {GLFW_KEY_ESCAPE,    COMMAND_PROMPT_BACK},
    },
    // justReleasedKey
    unordered_map<int, eFixedCommand> {},
    // repeatButton
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_UP,     COMMAND_PROMPT_UP},
        {BUTTON_LEFT,   COMMAND_PROMPT_LEFT},
        {BUTTON_DOWN,   COMMAND_PROMPT_DOWN},
        {BUTTON_RIGHT,  COMMAND_PROMPT_RIGHT},
    },
    // justPressedButton
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_A,      COMMAND_PROMPT_SELECT},
        {TRIGGER_RIGHT, COMMAND_PROMPT_SELECT},
        {BUTTON_START,  COMMAND_PROMPT_SELECT},
        {BUTTON_B,      COMMAND_PROMPT_BACK},
        {TRIGGER_LEFT,  COMMAND_PROMPT_BACK},
        {BUTTON_BACK,   COMMAND_PROMPT_BACK},
    },
    // justReleasedButton
    unordered_map<int, eFixedCommand> {}
)
{

    m_iCurrentPromptX = 0;
    m_iCurrentPromptY = 0;
}


void PromptMenu::setupKeyCommands()
{
}

void PromptMenu::executeKeyCommand(eHovercraft hovercraft, eFixedCommand command)
{

}

void PromptMenu::handleAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command)
{
}

void PromptMenu::executeAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command)
{
}

void PromptMenu::executeButtonFixedCommand(eHovercraft hovercraft, eFixedCommand command)
{
}

void PromptMenu::updateLeftStick(eHovercraft hovercraft, float x, float y)
{
}

void PromptMenu::updateRightStick(eHovercraft hovercraft, float x, float y)
{
}

/*
Joysticks can be used to nagivate the menu. Since joystick values are not
discrete in their direction like keys or buttons are, we need to be able to
find out what direction the joysticks are headed towards.

@param x    x-coordinate of joystick
@param y    y-coordinate of joystick
@return the prompt direction the joystick is moving in
        COMMAND_PROMPT_RIGHT
        COMMAND_PROMPT_LEFT
        COMMAND_PROMPT_UP
        COMMAND_PROMPT_DOWN
*/
eFixedCommand PromptMenu::joystickStateToPromptDirection(float x, float y)
{
    if (abs(x) > abs(y)) // x takes priority
    {
        return x > 0 ? COMMAND_PROMPT_RIGHT : COMMAND_PROMPT_LEFT;
    }
    else // y takes priority
    {
        return y > 0 ? COMMAND_PROMPT_UP : COMMAND_PROMPT_DOWN;
    }

}
