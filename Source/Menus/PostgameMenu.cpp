#include "Menus/PostgameMenu.h"
#include "Menus/MainMenu.h"
#include "UserInterface/MainInterface.h"
#include "GameStats.h"
#include "CommandHandler.h"

// Singleton instance
PostgameMenu* PostgameMenu::m_pInstance = nullptr;

PostgameMenu::PostgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            { "Quit Game", eFixedCommand::COMMAND_PROMPT_BACK },
        },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
}

Menu* PostgameMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new PostgameMenu();
    }
    return m_pInstance;
}

void PostgameMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    }
}

// Back returns to the mainmenu screen
void PostgameMenu::back()
{
    // For debug purposes, this quits the game instead of going back to the main menu
    PromptMenu::executeFixedCommand(HOVERCRAFT_INVALID, COMMAND_CLOSE_WINDOW);
    // m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
    // nextMenu(MainMenu::getInstance());
}

void PostgameMenu::enter()
{
    endGameStats = GAME_STATS->getEndGameStats();
    for (EndGameStat s : endGameStats)
    {
        // debug
        cout << "Hovercraft: " << s.hovercraft << " Points: " << s.afterAwardsScore << endl;
    }

    cout << "> " << getCurrentPrompt() << endl;
}
