#include "Menus/PostgameMenu.h"
#include "Menus/MainMenu.h"
#include "UserInterface/PostgameInterface.h"
#include "GameStats.h"
#include "CommandHandler.h"


// Unit: seconds
#define IGNORE_INPUT_TIME 2.0f
// Singleton instance
PostgameMenu* PostgameMenu::m_pInstance = nullptr;

PostgameMenu::PostgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            { "Main Menu", eFixedCommand::COMMAND_PROMPT_BACK },
        },
        {
            { "Awards", eFixedCommand::COMMAND_PROMPT_NEXT_MENU},
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
    if (m_bIgnoreUserInput)
    {
        return;
    }
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    case COMMAND_PROMPT_NEXT_MENU:
        //
        break;
    }
}

// Back returns to the mainmenu screen
void PostgameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void PostgameMenu::updateTimeValues(float fTimeInSeconds)
{
    m_fIgnoreUserInputTime -= fTimeInSeconds;

    if (m_fIgnoreUserInputTime <= 0)
    {
        m_bIgnoreUserInput = false;
    }
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

    m_bIgnoreUserInput = true;
    m_fIgnoreUserInputTime = IGNORE_INPUT_TIME;
}

