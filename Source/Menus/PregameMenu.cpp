#include "Menus/PregameMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/LoadingMenu.h"
#include "UserInterface/PregameInterface.h"
#include "CommandHandler.h"


// Interval of time to change game time
#define GAME_TIME_INTERVAL SECONDS_PER_MINUTE

// Default game time as pregame menu is first entered
// #define DEFAULT_GAME_TIME 3 * SECONDS_PER_MINUTE
#define DEFAULT_GAME_TIME 3 * GAME_TIME_INTERVAL
#define MAX_GAME_TIME 10 * GAME_TIME_INTERVAL
#define MIN_GAME_TIME 1 * GAME_TIME_INTERVAL

// Singleton instance
PregameMenu* PregameMenu::m_pInstance = nullptr;

PregameMenu::PregameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            // Even though start is visually on the bottom, we put it on top
            // so its defaulted to when we enter the menu
            { "Start Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU },
            // Even though back is visually on the left, we put start on the
            // left so its defaulted to when we move down
            { "Back", eFixedCommand::COMMAND_PROMPT_BACK }
        },
        {
            { "Player count", eFixedCommand::COMMAND_PROMPT_SELECT },
        },
        {
            { "Bot count", eFixedCommand::COMMAND_PROMPT_SELECT_2 },
        },
        {
            { "AI", eFixedCommand::COMMAND_PROMPT_SELECT_3 },
        },
        {
            { "Game time", eFixedCommand::COMMAND_PROMPT_SELECT_4 },
        },
        {
            { "Map", eFixedCommand::COMMAND_PROMPT_SELECT_5 },
        },
    }
)
{
    COMMAND_HANDLER->addMenu(this);
    // Note that this is in the constructor, not enter(). While there are
    // initial bot and player values, these SHOULD persist between menu changes
    // during runtime because it's annoying to have the values changed away
    // from what the player chose.
#ifdef NDEBUG
    m_iPlayerCount = FuncUtils::bound(INPUT_HANDLER->getJoystickCount(),
                                      MIN_PLAYER_COUNT, MAX_PLAYER_COUNT);
    m_iBotCount = m_iPlayerCount;
#else
    m_iBotCount = 0;
    m_iPlayerCount = 2;
#endif // NDEBUG

    m_eAIType = AI_ON_SAME_TEAM;
    m_fGameTime = DEFAULT_GAME_TIME;
    m_iMapNumber = MIN_MAP_NUMBER;
}

Menu* PregameMenu::getInstance()
{
    if (nullptr == m_pInstance) {
        m_pInstance = new PregameMenu();
    }
    return m_pInstance;
}

void PregameMenu::select(eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_BACK:
        back();
        break;
    case COMMAND_PROMPT_NEXT_MENU:
        m_pGameManager->initializeNewGame(m_iPlayerCount,
                                          m_iBotCount,
                                          static_cast<float>(m_fGameTime),
                                          m_eAIType,
                                          m_iMapNumber);
        break;
    }
}

// Back returns to the mainmenu screen
void PregameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void PregameMenu::enter()
{
    PromptMenu::enter();
    m_pGameManager->setCurrentInterface(PregameInterface::getInstance(m_pGameManager->getWidth(),
                                                                   m_pGameManager->getHeight()));

}

void PregameMenu::moveCursor(eFixedCommand direction)
{
    PromptMenu::moveCursor(direction);
    switch (getCurrentPromptCommand())
    {
        case eFixedCommand::COMMAND_PROMPT_SELECT: // Player

            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iPlayerCount = FuncUtils::subtractModulo(m_iPlayerCount, 1,
                                                           MIN_PLAYER_COUNT, MAX_PLAYER_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iPlayerCount = FuncUtils::addModulo(m_iPlayerCount, 1,
                                                      MIN_PLAYER_COUNT, MAX_PLAYER_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_2: // Bot
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iBotCount = FuncUtils::subtractModulo(m_iBotCount, 1,
                                                        MIN_BOT_COUNT, MAX_BOT_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iBotCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iBotCount = FuncUtils::addModulo(m_iBotCount, 1,
                                                   MIN_BOT_COUNT, MAX_BOT_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iBotCount << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_3: // AI
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
            case COMMAND_PROMPT_RIGHT:
                // Currently there are only 2 AI configurations, so we will
                // toggle between them.
                if (m_eAIType == AI_ON_SAME_TEAM)
                {
                    m_eAIType = AI_SOLO;
                }
                else
                {
                    m_eAIType = AI_ON_SAME_TEAM;
                }
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_eAIType << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_4: // game time
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_fGameTime = FuncUtils::bound(m_fGameTime - GAME_TIME_INTERVAL,
                                               MIN_GAME_TIME, MAX_GAME_TIME);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << FuncUtils::timeToString(m_fGameTime) << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_fGameTime = FuncUtils::bound(m_fGameTime + GAME_TIME_INTERVAL,
                                               MIN_GAME_TIME, MAX_GAME_TIME);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << FuncUtils::timeToString(m_fGameTime) << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_5: // Map

            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iMapNumber = FuncUtils::subtractModulo(m_iMapNumber, 1,
                                                         MIN_MAP_NUMBER, MAX_MAP_NUMBER);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iMapNumber << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iMapNumber = FuncUtils::addModulo(m_iMapNumber, 1,
                                                    MIN_MAP_NUMBER, MAX_MAP_NUMBER);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iMapNumber << endl;
                break;
            }
            break;
    }
}


