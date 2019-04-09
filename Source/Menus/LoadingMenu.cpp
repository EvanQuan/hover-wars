#include "Menus/MainMenu.h"
#include "Menus/LoadingMenu.h"
#include "UserInterface/LoadingInterface.h"
#include "CommandHandler.h"

// Singleton instance
LoadingMenu* LoadingMenu::m_pInstance = nullptr;

LoadingMenu::LoadingMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Loading", eFixedCommand::COMMAND_INVALID_FIXED},  },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

Menu* LoadingMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new LoadingMenu();
    }
    return m_pInstance;
}

void LoadingMenu::select(eFixedCommand command)
{
}

void LoadingMenu::enter()
{
    PromptMenu::enter();
    m_pGameManager->setCurrentInterface(LoadingInterface::getInstance(m_pGameManager->getWidth(), m_pGameManager->getHeight()));
}
