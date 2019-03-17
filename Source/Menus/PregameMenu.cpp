#include "Menus/PregameMenu.h"
#include "Menus/MainMenu.h"
#include "InputHandler.h"

// Singleton instance
PregameMenu* PregameMenu::m_pInstance = nullptr;

PregameMenu::PregameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        // TODO
        { {"New Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
        { {"Exit", eFixedCommand::COMMAND_CLOSE_WINDOW},  },
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
    case COMMAND_PROMPT_NEXT_MENU:
        // TODO
        break;
    }
}

// Back returns to the start screen
void PregameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}
