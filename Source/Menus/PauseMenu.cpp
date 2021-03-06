#include "Menus/PauseMenu.h"
#include "Menus/GameMenu.h"
#include "Menus/MainMenu.h"
#include "UserInterface/PauseInterface.h"
#include "Menus/MenuManager.h"

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
    MENU_MANAGER->addMenu(this);
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

void PauseMenu::enterOverride()
{ 
    GAME_MANAGER->setPaused(true);
    m_pUserInterfaceManager->setCurrentInterface(PauseInterface::getInstance());
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
