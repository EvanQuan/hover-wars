#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"
#include "Menus/PregameMenu.h"
#include "Menus/GameMenu.h"

// Singleton instance
MainMenu* MainMenu::m_pInstance = nullptr;

MainMenu::MainMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"New Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},
          {"Exit", eFixedCommand::COMMAND_CLOSE_WINDOW}, },
    }
)
{

}

Menu* MainMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new MainMenu();
    }
    return m_pInstance;
}

void MainMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        nextMenu(GameMenu::getInstance());
        // Should go to PregameMenu, but for now goes straight to ingame
        // nextMenu(PregameMenu::getInstance());
        break;
    case COMMAND_CLOSE_WINDOW:
        nextMenu(MainMenu::getInstance());
        break;
    }
}

// Back returns to the start screen
void MainMenu::back()
{
    nextMenu(StartMenu::getInstance());
}
