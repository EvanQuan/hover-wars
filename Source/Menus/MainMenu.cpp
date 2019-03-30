#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"
#include "Menus/PregameMenu.h"
#include "Menus/ControlsMenu.h"
#include "Menus/RulesMenu.h"
#include "UserInterface/PregameInterface.h"
#include "CommandHandler.h"

// Singleton instance
MainMenu* MainMenu::m_pInstance = nullptr;

MainMenu::MainMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            {"New Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},
        },
        {
            {"Rules", eFixedCommand::COMMAND_PROMPT_SELECT},
        },
        {
            {"Controls", eFixedCommand::COMMAND_PROMPT_SELECT_2},
        },
        {
            {"Quit", eFixedCommand::COMMAND_CLOSE_WINDOW},
        },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

Menu* MainMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new MainMenu();
    }
    return m_pInstance;
}

void MainMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_NEXT_MENU:
        m_pGameManager->setCurrentInterface(PregameInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(PregameMenu::getInstance());
        break;
    case COMMAND_PROMPT_SELECT:
        // m_pGameManager->setCurrentInterface(PregameInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(RulesMenu::getInstance());
        break;
    case COMMAND_PROMPT_SELECT_2:
        // m_pGameManager->setCurrentInterface(PregameInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(ControlsMenu::getInstance());
        break;
    case COMMAND_CLOSE_WINDOW:
        PromptMenu::executeFixedCommand(HOVERCRAFT_INVALID, COMMAND_CLOSE_WINDOW);
        break;
    }
}

// Back returns to the start screen
void MainMenu::back()
{
    nextMenu(StartMenu::getInstance());
}
