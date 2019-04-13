#include "Menus/MainMenu.h"
#include "Menus/ControlsMenu.h"
#include "Menus/MenuManager.h"
#include "UserInterface/ControlsInterface.h"

// Singleton instance
ControlsMenu* ControlsMenu::m_pInstance = nullptr;

ControlsMenu::ControlsMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Return To Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    MENU_MANAGER->addMenu(this);
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
        nextMenu(MainMenu::getInstance());
        break;
    }
}

void ControlsMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void ControlsMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(ControlsInterface::getInstance(m_pGameManager->getWidth(), m_pGameManager->getHeight()));
}


