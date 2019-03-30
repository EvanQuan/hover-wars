#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"
#include "UserInterface/MainInterface.h"
#include "CommandHandler.h"

// Singleton instance
StartMenu* StartMenu::m_pInstance = nullptr;

StartMenu::StartMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Insert Coin", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
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
        m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(MainMenu::getInstance());
        break;
    }
}
