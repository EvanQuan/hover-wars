#include "Menus/PostgameMenu.h"
#include "Menus/MainMenu.h"
#include "InputHandler.h"

// Singleton instance
PostgameMenu* PostgameMenu::m_pInstance = nullptr;

PostgameMenu::PostgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            { "Return to Main Menu", eFixedCommand::COMMAND_PROMPT_BACK },
        },
    }
)
{
}

Menu* PostgameMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new PostgameMenu();
    }
    return m_pInstance;
}

void PostgameMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    }
}

// Back returns to the mainmenu screen
void PostgameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}
