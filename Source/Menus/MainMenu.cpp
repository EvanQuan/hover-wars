#include "Menus/MainMenu.h"
#include "Menus/StartMenu.h"
#include "Menus/PregameMenu.h"
#include "Menus/ControlsMenu.h"
#include "Menus/SettingsMenu.h"
#include "Menus/RulesMenu.h"
#include "UserInterface/PregameInterface.h"
#include "UserInterface/RulesInterface.h"
#include "UserInterface/ControlsInterface.h"
#include "UserInterface/MainInterface.h"
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
        // {
            // {"Settings", eFixedCommand::COMMAND_PROMPT_SELECT_3},
        // },
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
        nextMenu(PregameMenu::getInstance());
        break;
    case COMMAND_PROMPT_SELECT:
        nextMenu(RulesMenu::getInstance());
        break;
    case COMMAND_PROMPT_SELECT_2:
        nextMenu(ControlsMenu::getInstance());
        break;
    case COMMAND_PROMPT_SELECT_3:
        nextMenu(SettingsMenu::getInstance());
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

void MainMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->getWidth(), m_pGameManager->getHeight()));
}
