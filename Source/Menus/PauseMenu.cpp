#include "Menus/PauseMenu.h"
#include "Menus/GameMenu.h"
#include "CommandHandler.h"

// Singleton instance
PauseMenu* PauseMenu::m_pInstance = nullptr;

PauseMenu::PauseMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            {"Continue", eFixedCommand::COMMAND_MENU_PAUSE_TOGGLE},
            {"Quit", eFixedCommand::COMMAND_CLOSE_WINDOW}
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
        GAME_MANAGER->setPaused(false);
        SOUND_MANAGER->togglePaused();
        break;
    case COMMAND_CLOSE_WINDOW:
        glfwSetWindowShouldClose(COMMAND_HANDLER->m_pWindow, GL_TRUE);
        break;
    }
}


void PauseMenu::back()
{
    select(eFixedCommand::COMMAND_MENU_PAUSE_TOGGLE);
}
