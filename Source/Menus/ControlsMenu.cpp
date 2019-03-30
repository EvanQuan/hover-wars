#include "Menus/MainMenu.h"
#include "Menus/ControlsMenu.h"
#include "UserInterface/MainInterface.h"

// Singleton instance
ControlsMenu* ControlsMenu::m_pInstance = nullptr;

ControlsMenu::ControlsMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Return To Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
}

Menu* ControlsMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new ControlsMenu();
    }
    return m_pInstance;
}

void ControlsMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(MainMenu::getInstance());
        break;
    }
}
