#include "Menus/PostgameMenu.h"
#include "Menus/MainMenu.h"
#include "UserInterface/PostgameInterface.h"
#include "GameStats.h"
#include "CommandHandler.h"

// Singleton instance
PostgameMenu* PostgameMenu::m_pInstance = nullptr;

PostgameMenu::PostgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            { "Main Menu", eFixedCommand::COMMAND_PROMPT_BACK },
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
    nextMenu(MainMenu::getInstance());
}

void PostgameMenu::enter()
{
    PromptMenu::enter();
    m_pGameManager->setCurrentInterface(PostgameInterface::getInstance(m_pGameManager->getWidth(),
                                                                   m_pGameManager->getHeight()));
    endGameStats = GAME_STATS->getEndGameStats();
    for (EndGameStat s : endGameStats)
    {
        // debug
        cout << "Hovercraft: " << s.hovercraft << " Points: " << s.afterAwardsScore << endl;
    }

    cout << "> " << getCurrentPrompt() << endl;
}
