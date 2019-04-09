#include "Menus/MainMenu.h"
#include "Menus/RulesMenu.h"
#include "UserInterface/RulesInterface.h"
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
        nextMenu(MainMenu::getInstance());
        break;
    }
}

void RulesMenu::enter()
{
    PromptMenu::enter();
    m_pGameManager->setCurrentInterface(RulesInterface::getInstance(m_pGameManager->getWidth(),
                                                                    m_pGameManager->getHeight()));
}
