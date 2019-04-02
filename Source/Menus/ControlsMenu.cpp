#include "Menus/MainMenu.h"
#include "Menus/ControlsMenu.h"
#include "UserInterface/MainInterface.h"
#include "CommandHandler.h"

// Singleton instance
ControlsMenu* ControlsMenu::m_pInstance = nullptr;

ControlsMenu::ControlsMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Return To Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
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
        m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->getWidth(), m_pGameManager->getHeight()));
        nextMenu(MainMenu::getInstance());
        break;
    }
}
