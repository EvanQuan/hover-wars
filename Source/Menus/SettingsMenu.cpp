#include "Menus/SettingsMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/LoadingMenu.h"
// #include "UserInterface/SettingsInterface.h"
#include "CommandHandler.h"


// Interval of time to change game time
#define GAME_TIME_INTERVAL SECONDS_PER_MINUTE

// Default game time as pregame menu is first entered
// #define DEFAULT_GAME_TIME 3 * SECONDS_PER_MINUTE
#define DEFAULT_GAME_TIME 3 * GAME_TIME_INTERVAL
#define MAX_GAME_TIME 10 * GAME_TIME_INTERVAL
#define MIN_GAME_TIME 1 * GAME_TIME_INTERVAL

// Singleton instance
SettingsMenu* SettingsMenu::m_pInstance = nullptr;

SettingsMenu::SettingsMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        // Logically, back is selected by default first, although visually it
        // should appear last in the SettingsInterface
        {
            { "Back", eFixedCommand::COMMAND_PROMPT_BACK }
        },
        {
            { "Music", eFixedCommand::COMMAND_PROMPT_SELECT }
        },
    }
)
{
    COMMAND_HANDLER->addMenu(this);

    // Default setting values
    // As of now, these values are reset every game launch. Perhaps later,
    // these settings can persist by being stored in a file.
    m_bMusicEnabled = true;
}

SettingsMenu* SettingsMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new SettingsMenu();
    }
    return m_pInstance;
}

void SettingsMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    }
}

// Back returns to the mainmenu screen
void SettingsMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void SettingsMenu::enter()
{
    PromptMenu::enter();
    // m_pGameManager->setCurrentInterface(SettingsInterface::getInstance(m_pGameManager->getWidth(),
      //                                                             m_pGameManager->getHeight()));

}

bool SettingsMenu::moveCursorOverride(eFixedCommand direction)
{
    switch (getCurrentPromptCommand())
    {
        case eFixedCommand::COMMAND_PROMPT_SELECT: // Player

            switch (direction)
            {
            case eFixedCommand::COMMAND_PROMPT_RIGHT:
            case eFixedCommand::COMMAND_PROMPT_LEFT:
                m_bMusicEnabled = !m_bMusicEnabled;
                cout << "music enabled: " << m_bMusicEnabled << endl;
                return true;
            }
    }
    return false;
}


