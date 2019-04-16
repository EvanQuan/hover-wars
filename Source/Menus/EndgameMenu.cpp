#include "Menus/MainMenu.h"
#include "Menus/EndgameMenu.h"
#include "UserInterface/EndgameInterface.h"
#include "Menus/MenuManager.h"

// Singleton instance
EndgameMenu* EndgameMenu::m_pInstance = nullptr;

EndgameMenu::EndgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Return to Main Menu", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    MENU_MANAGER->addMenu(this);
}

Menu* EndgameMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new EndgameMenu();
    }
    return m_pInstance;
}

void EndgameMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        nextMenu(MainMenu::getInstance());
        break;
    }
}

void EndgameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void EndgameMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(EndgameInterface::getInstance(m_pGameManager->getWidth(),
                                                                    m_pGameManager->getHeight()));
}
