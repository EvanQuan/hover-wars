#include "Menus/PauseMenu.h"
#include "Menus/GameMenu.h"
#include "Menus/MainMenu.h"
#include "UserInterface/PauseInterface.h"
#include "CommandHandler.h"

// Singleton instance
PauseMenu* PauseMenu::m_pInstance = nullptr;

PauseMenu::PauseMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            {"Continue", eFixedCommand::COMMAND_MENU_PAUSE_TOGGLE},
        },
        {
            {"End Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU}
        },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

Menu* PauseMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new PauseMenu();
    }
    return m_pInstance;
}

void PauseMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_MENU_PAUSE_TOGGLE:
        nextMenu(GameMenu::getInstance());
        break;
    case COMMAND_PROMPT_NEXT_MENU:
        m_pGameManager->endGame();
        break;
    }
}


void PauseMenu::back()
{
    select(eFixedCommand::COMMAND_MENU_PAUSE_TOGGLE);
}

void PauseMenu::enter()
{ 
    PromptMenu::enter();
    GAME_MANAGER->setPaused(true);
    m_pGameManager->setCurrentInterface(PauseInterface::getInstance(m_pGameManager->getWidth(),
                                                                   m_pGameManager->getHeight()));
}

/*
    Only let the pauser affect the pause menu.
*/
void PauseMenu::executeFixedCommand(eHovercraft hovercraft, eFixedCommand command)
{
    if (hovercraft == m_ePauser)
    {
        PromptMenu::executeFixedCommand(hovercraft, command);
    }
}
