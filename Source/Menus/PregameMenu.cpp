#include "Menus/PregameMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/GameMenu.h"
#include "InputHandler.h"

// Singleton instance
PregameMenu* PregameMenu::m_pInstance = nullptr;

PregameMenu::PregameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            {"Player count", eFixedCommand::COMMAND_INVALID_FIXED},
        },
        {
            { "Bot count", eFixedCommand::COMMAND_INVALID_FIXED },
                // Even though back is visually on the left, we put start on the
                // left so its defaulted to when we move down
        },
        {
            { "Start Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU },
            {"Back", eFixedCommand::COMMAND_PROMPT_BACK}
        },
    }
)
{
    // Note that this is in the constructor, not enter(). While there are
    // initial bot and player values, these SHOULD persist between menu changes
    // during runtime because it's annoying to have the values changed away
    // from what the player chose.
    m_iBotCount = MAX_BOT_COUNT;
    m_iPlayerCount = INPUT_HANDLER->getJoystickCount();
}

Menu* PregameMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new PregameMenu();
    }
    return m_pInstance;
}

void PregameMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    case COMMAND_PROMPT_NEXT_MENU:
        nextMenu(GameMenu::getInstance());
        break;
    }
}

// Back returns to the mainmenu screen
void PregameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void PregameMenu::moveCursor(eFixedCommand direction)
{
    PromptMenu::moveCursor(direction);
    // switch (getCurrentPrompt())
    // {

    // }
}
