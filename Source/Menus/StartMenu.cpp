#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"

// Singleton instance
StartMenu* StartMenu::m_pInstance = nullptr;

StartMenu::StartMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Press Start", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{

}

Menu* StartMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new StartMenu();
    }
    return m_pInstance;
}

void StartMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        nextMenu(MainMenu::getInstance());
        break;
    }
}
