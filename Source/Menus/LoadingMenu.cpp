#include "Menus/MainMenu.h"
#include "Menus/LoadingMenu.h"
#include "UserInterface/LoadingInterface.h"
#include "Menus/MenuManager.h"

// Singleton instance
LoadingMenu* LoadingMenu::m_pInstance = nullptr;

LoadingMenu::LoadingMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Loading", eFixedCommand::COMMAND_INVALID_FIXED},  },
    }
)
{
    MENU_MANAGER->addMenu(this);
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

void LoadingMenu::enterOverride()
{
    m_pUserInterfaceManager->setCurrentInterface(LoadingInterface::getInstance());
}
