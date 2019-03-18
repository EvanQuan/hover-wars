#include "Menus/PauseMenu.h"
#include "Menus/GameMenu.h"
#include "Menus/MainMenu.h"

// Singleton instance
PauseMenu* PauseMenu::m_pInstance = nullptr;

PauseMenu::PauseMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            {"Continue", eFixedCommand::COMMAND_MENU_PAUSE_TOGGLE},
        },
        {
            {"Return to Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU}
        },
    }
)
{

}

Menu* PauseMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new PauseMenu();
    }
    return m_pInstance;
}

void PauseMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_MENU_PAUSE_TOGGLE:
        nextMenu(GameMenu::getInstance());
        SOUND_MANAGER->togglePaused();
        break;
    case COMMAND_PROMPT_NEXT_MENU:
        nextMenu(MainMenu::getInstance());
        SOUND_MANAGER->togglePaused();
        break;
    }
}


void PauseMenu::back()
{
    select(eFixedCommand::COMMAND_MENU_PAUSE_TOGGLE);
}

void PauseMenu::enter()
{ 
    PromptMenu::enter();
    GAME_MANAGER->setPaused(true);
    SOUND_MANAGER->togglePaused();
}
