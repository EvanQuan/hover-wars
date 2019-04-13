#include "Menus/MainMenu.h"
#include "Menus/RulesMenu.h"
#include "UserInterface/RulesInterface.h"
#include "Menus/MenuManager.h"

// Singleton instance
RulesMenu* RulesMenu::m_pInstance = nullptr;

RulesMenu::RulesMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Return to Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    MENU_MANAGER->addMenu(this);
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

void RulesMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void RulesMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(RulesInterface::getInstance(m_pGameManager->getWidth(),
                                                                    m_pGameManager->getHeight()));
}
