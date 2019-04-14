#include "Menus/PregameMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/LoadingMenu.h"
#include "UserInterface/PregameInterface.h"
#include "Menus/MenuManager.h"


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
            { "Bot difficulty", eFixedCommand::COMMAND_PROMPT_SELECT_6 },
        },
        {
            { "Game time", eFixedCommand::COMMAND_PROMPT_SELECT_4 },
        },
        {
            { "Game mode", eFixedCommand::COMMAND_PROMPT_SELECT_3 },
        },
        {
            { "Score loss", eFixedCommand::COMMAND_PROMPT_SELECT_7 },
        },
        {
            { "Map", eFixedCommand::COMMAND_PROMPT_SELECT_5 },
        },
    }
)
{
    MENU_MANAGER->addMenu(this);
    // Note that this is in the constructor, not enterOverride(). While there are
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

    m_eBotDifficulty = DIFFICULTY_MEDIUM;
    m_eGameMode = GAMEMODE_TEAM_BOTS_VS_SOLO_PLAYERS;
    m_fGameTime = DEFAULT_GAME_TIME;
    m_bScoreLossEnabled = true;
    m_iMapNumber = MIN_MAP_NUMBER;
}

PregameMenu* PregameMenu::getInstance()
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
        // Force 4 players if playing player teams
        m_pGameManager->initializeNewGame(getGameMode() == GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS ? MAX_PLAYER_COUNT : m_iPlayerCount,
                                          m_iBotCount,
                                          getBotDifficulty(),
                                          static_cast<float>(m_fGameTime),
                                          getGameMode(),
                                          m_iMapNumber,
                                          m_bScoreLossEnabled);
        break;
    }
}

// Back returns to the mainmenu screen
void PregameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void PregameMenu::enterOverride()
{
    m_pGameManager->setCurrentInterface(PregameInterface::getInstance(m_pGameManager->getWidth(),
                                                                   m_pGameManager->getHeight()));

}

bool PregameMenu::moveCursorOverride(eFixedCommand direction)
{
    switch (getCurrentPromptCommand())
    {
        case eFixedCommand::COMMAND_PROMPT_SELECT: // Player

            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iPlayerCount = FuncUtils::subtractModulo(m_iPlayerCount, 1,
                                                           MIN_PLAYER_COUNT, MAX_PLAYER_COUNT);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iPlayerCount = FuncUtils::addModulo(m_iPlayerCount, 1,
                                                      MIN_PLAYER_COUNT, MAX_PLAYER_COUNT);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            default:
                return false;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_2: // Bot
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iBotCount = FuncUtils::subtractModulo(m_iBotCount, 1,
                                                        MIN_BOT_COUNT, MAX_BOT_COUNT);
                cout << "\t" << m_iBotCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iBotCount = FuncUtils::addModulo(m_iBotCount, 1,
                                                   MIN_BOT_COUNT, MAX_BOT_COUNT);
                cout << "\t" << m_iBotCount << endl;
                break;
            default:
                return false;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_3: // Game mode
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_eGameMode = FuncUtils::subtractModulo(m_eGameMode, 1,
                                                        0, GAMEMODE_COUNT - 1);
                cout << "\t" << m_eGameMode << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_eGameMode = FuncUtils::addModulo(m_eGameMode, 1,
                                                        0, GAMEMODE_COUNT - 1);
                cout << "\t" << m_eGameMode << endl;
                break;
            default:
                return false;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_6: // bot difficulty
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_eBotDifficulty = FuncUtils::subtractModulo(m_eBotDifficulty, 1,
                                                        DIFFICULTY_EASY, DIFFICULTY_HARD);
                cout << "\t" << m_eBotDifficulty << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_eBotDifficulty = FuncUtils::addModulo(m_eBotDifficulty, 1,
                                                        DIFFICULTY_EASY, DIFFICULTY_HARD);
                cout << "\t" << m_eBotDifficulty << endl;
                break;
            default:
                return false;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_4: // game time
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_fGameTime = FuncUtils::bound(m_fGameTime - GAME_TIME_INTERVAL,
                                               MIN_GAME_TIME, MAX_GAME_TIME);
                cout << "\t" << FuncUtils::timeToString(m_fGameTime) << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_fGameTime = FuncUtils::bound(m_fGameTime + GAME_TIME_INTERVAL,
                                               MIN_GAME_TIME, MAX_GAME_TIME);
                cout << "\t" << FuncUtils::timeToString(m_fGameTime) << endl;
                break;
            default:
                return false;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_5: // Map

            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iMapNumber = FuncUtils::subtractModulo(m_iMapNumber, 1,
                                                         MIN_MAP_NUMBER, MAX_MAP_NUMBER);
                cout << "\t" << m_iMapNumber << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iMapNumber = FuncUtils::addModulo(m_iMapNumber, 1,
                                                    MIN_MAP_NUMBER, MAX_MAP_NUMBER);
                cout << "\t" << m_iMapNumber << endl;
                break;
            default:
                return false;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_7: // Score loss

            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
            case COMMAND_PROMPT_RIGHT:
                m_bScoreLossEnabled = !m_bScoreLossEnabled;
                cout << "\t" << m_bScoreLossEnabled << endl;
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_SELECT);
                break;
            default:
                return false;
            }
            break;
        default:
            return false;
    }
    return true;
}


