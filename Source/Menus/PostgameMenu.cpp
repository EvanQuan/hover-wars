#include "Menus/PostgameMenu.h"
#include "Menus/AwardsMenu.h"
#include "Menus/MainMenu.h"
#include "UserInterface/PostgameInterface.h"
#include "GameStats.h"
#include "Menus/MenuManager.h"


// Unit: seconds
// Disable for now
#define IGNORE_INPUT_TIME 0.0f
// Singleton instance
PostgameMenu* PostgameMenu::m_pInstance = nullptr;

PostgameMenu::PostgameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        // We select awards first to prevent the user from accidentally going
        // instantly to the main menu as the game ends
        {
            { "Awards", eFixedCommand::COMMAND_PROMPT_NEXT_MENU },
        },
        {
            { "Return to Main Menu", eFixedCommand::COMMAND_PROMPT_BACK },
        },
    }
)
{
    MENU_MANAGER->addMenu(this);
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
    //jif (m_bIgnoreUserInput)
    //j{
        //jreturn;
    //j}
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    case COMMAND_PROMPT_NEXT_MENU:
        nextMenu(AwardsMenu::getInstance());
        break;
    }
}

// Back returns to the mainmenu screen
void PostgameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void PostgameMenu::enterOverride()
{
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

