#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"
#include "UserInterface/StartInterface.h"
#include "CommandHandler.h"

// Singleton instance
StartMenu* StartMenu::m_pInstance = nullptr;

StartMenu::StartMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        { {"Insert Coin", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},  },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

Menu* StartMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new StartMenu();
    }
    return m_pInstance;
}

void StartMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_INSERT_COIN);
        nextMenu(MainMenu::getInstance());
        break;
    }
}

void StartMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(StartInterface::getInstance(m_pGameManager->getWidth(),
                                                                   m_pGameManager->getHeight()));
}
