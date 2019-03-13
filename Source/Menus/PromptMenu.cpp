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
    unordered_map<eVariableCommand, const char*> {},
    unordered_map<int, eFixedCommand> {},
    unordered_map<int, eFixedCommand> {},
    unordered_map<int, eFixedCommand> {},
    unordered_map<int, eFixedCommand> {},
    unordered_map<int, eFixedCommand> {},
    unordered_map<int, eFixedCommand> {}
)
{

    m_iCurrentPromptX = 0;
    m_iCurrentPromptY = 0;
}

void PromptMenu::updateKeyboardCommands()
{
    map<int, InputHandler::eInputState> keys = m_pInputHandler->m_keys;
    for (auto it : keys)
    {
        switch (it.second)
        {
        case InputHandler::INPUT_JUST_PRESSED:
            break;
        case InputHandler::INPUT_PRESSED:
            break;
        case InputHandler::INPUT_JUST_RELEASED:
            m_pInputHandler->m_keys.erase(it.first);
            break;
        }
    }
}

void PromptMenu::updateJoystickCommands()
{
    m_pInputHandler->updateJoysticks();

    for (int joystickID = GLFW_JOYSTICK_1;
        joystickID < MAX_PLAYER_JOYSTICK;
        joystickID++)
    {
        int joystickIsPresent = m_pInputHandler->m_pJoystickIsPresent[joystickID];
    }
}
