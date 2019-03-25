#include "Menus/PregameMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/GameMenu.h"
#include "UserInterface/GameInterface.h"
#include "UserInterface/MainInterface.h"

// Default game time as pregame menu is first entered
// #define DEFAULT_GAME_TIME 3 * SECONDS_PER_MINUTE
#define DEFAULT_GAME_TIME 3 * SECONDS_PER_MINUTE // debug
#define MAX_GAME_TIME 4 * SECONDS_PER_MINUTE
#define MIN_GAME_TIME 1 * SECONDS_PER_MINUTE

// Interval of time to change game time
#define GAME_TIME_INTERVAL 30

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
            { "Game time", eFixedCommand::COMMAND_PROMPT_SELECT_3 },
        },
    }
)
{
    // Note that this is in the constructor, not enter(). While there are
    // initial bot and player values, these SHOULD persist between menu changes
    // during runtime because it's annoying to have the values changed away
    // from what the player chose.
    m_iBotCount = MAX_BOT_COUNT;
    m_iPlayerCount = FuncUtils::max(1, INPUT_HANDLER->getJoystickCount());
    m_fGameTime = DEFAULT_GAME_TIME;
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
        // Let GameManager initialize a new game before switching to game menu
        // controls
        m_pGameManager->initializeNewGame(m_iPlayerCount, m_iBotCount, static_cast<float>(m_fGameTime), RELEASE_ENV);
        m_pGameManager->setCurrentInterface(GameInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
        nextMenu(GameMenu::getInstance());
        break;
    }
}

// Back returns to the mainmenu screen
void PregameMenu::back()
{
    m_pGameManager->setCurrentInterface(MainInterface::getInstance(m_pGameManager->m_iWidth, m_pGameManager->m_iHeight));
    nextMenu(MainMenu::getInstance());
}

void PregameMenu::moveCursor(eFixedCommand direction)
{
    int maxPlayerCount;
    PromptMenu::moveCursor(direction);
    switch (getCurrentPromptCommand())
    {
        case eFixedCommand::COMMAND_PROMPT_SELECT: // Player
            maxPlayerCount = FuncUtils::bound(INPUT_HANDLER->getJoystickCount() + 1, MIN_PLAYER_COUNT, MAX_PLAYER_COUNT); // add 1 for keyboard
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iPlayerCount = FuncUtils::subtractModulo(m_iPlayerCount, 1, MIN_PLAYER_COUNT, maxPlayerCount);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iPlayerCount = FuncUtils::addModulo(m_iPlayerCount, 1, MIN_PLAYER_COUNT, maxPlayerCount);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_2: // Bot
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iBotCount = FuncUtils::subtractModulo(m_iBotCount, 1, MIN_BOT_COUNT, MAX_BOT_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iBotCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iBotCount = FuncUtils::addModulo(m_iBotCount, 1, MIN_BOT_COUNT, MAX_BOT_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iBotCount << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_3: // game time
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_fGameTime = FuncUtils::bound(m_fGameTime - GAME_TIME_INTERVAL, MIN_GAME_TIME, MAX_GAME_TIME);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << FuncUtils::timeToString(m_fGameTime) << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_fGameTime = FuncUtils::bound(m_fGameTime + GAME_TIME_INTERVAL, MIN_GAME_TIME, MAX_GAME_TIME);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << FuncUtils::timeToString(m_fGameTime) << endl;
                break;
            }
    }
}


