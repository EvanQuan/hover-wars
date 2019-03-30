#include "Menus/MainMenu.h"
#include "Menus/RulesMenu.h"
#include "UserInterface/MainInterface.h"
#include "CommandHandler.h"

// Singleton instance
RulesMenu* RulesMenu::m_pInstance = nullptr;

RulesMenu::RulesMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Return to Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

Menu* RulesMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new RulesMenu();
    }
    return m_pInstance;
}

void RulesMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(MainMenu::getInstance());
        break;
    }
}
