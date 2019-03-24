#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"
#include "Menus/PregameMenu.h"
#include "UserInterface/PregameInterface.h"

// Singleton instance
MainMenu* MainMenu::m_pInstance = nullptr;

MainMenu::MainMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            {"New Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},
        },
        {
            {"Quit", eFixedCommand::COMMAND_CLOSE_WINDOW},
        },
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
        m_pGameManager->setCurrentInterface(PregameInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(PregameMenu::getInstance());
        break;
    case COMMAND_CLOSE_WINDOW:
        PromptMenu::executeFixedCommand(HOVERCRAFT_INVALID, COMMAND_CLOSE_WINDOW);
        break;
    }
}

// Back returns to the start screen
void MainMenu::back()
{
    nextMenu(StartMenu::getInstance());
}
