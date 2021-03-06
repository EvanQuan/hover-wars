#include "Menus/MainMenu.h"
#include "Menus/AwardsMenu.h"
#include "Menus/PostgameMenu.h"
#include "GameStats.h"
#include "UserInterface/AwardsInterface.h"
#include "Menus/MenuManager.h"

// Singleton instance
AwardsMenu* AwardsMenu::m_pInstance = nullptr;

/*
    Shows awards. Retruns back to post game menu
*/
AwardsMenu::AwardsMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Back", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    MENU_MANAGER->addMenu(this);
}

Menu* AwardsMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new AwardsMenu();
    }
    return m_pInstance;
}

void AwardsMenu::back()
{
    nextMenu(PostgameMenu::getInstance());
}

void AwardsMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        back();
        break;
    }
}

void AwardsMenu::enterOverride()
{
    m_pUserInterfaceManager->setCurrentInterface(AwardsInterface::getInstance());
}
