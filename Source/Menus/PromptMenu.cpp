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

PromptMenu::PromptMenu()
{
    m_iCurrentPromptX = 0;
    m_iCurrentPromptY = 0;
}

void PromptMenu::updateKeyboardCommands()
{
}

void PromptMenu::updateJoystickCommands()
{
}
