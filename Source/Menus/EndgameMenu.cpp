#include "Menus/MainMenu.h"
#include "Menus/EndgameMenu.h"
#include "Menus/PostgameMenu.h"
#include "UserInterface/EndgameInterface.h"
#include "Menus/MenuManager.h"

#define DELAY_TIME 2.0f

// Singleton instance
EndgameMenu* EndgameMenu::m_pInstance = nullptr;

EndgameMenu::EndgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        // Cannot do anything in the endgame menu
        { {"", eFixedCommand::COMMAND_INVALID_FIXED},  },
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
}

void EndgameMenu::back()
{
}

void EndgameMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(EndgameInterface::getInstance(m_pGameManager->getWidth(),
                                                                      m_pGameManager->getHeight()));
    m_fDelayTime = DELAY_TIME;
}

void EndgameMenu::updateTimeValueOverride(float fTimeInSeconds)
{
    m_fDelayTime -= fTimeInSeconds;
    if (m_fDelayTime < 0)
    {
        nextMenu(PostgameMenu::getInstance());
    }
}
