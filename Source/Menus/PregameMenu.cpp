#include "Menus/PregameMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/GameMenu.h"
#include "InputHandler.h"

// Singleton instance
PregameMenu* PregameMenu::m_pInstance = nullptr;

PregameMenu::PregameMenu() : PromptMenu(
    vector < vector<pair<const char*, eFixedCommand>> >
    {
        {
            { "Player count", eFixedCommand::COMMAND_PROMPT_SELECT },
        },
        {
            { "Bot count", eFixedCommand::COMMAND_PROMPT_SELECT_2 },
                // Even though back is visually on the left, we put start on the
                // left so its defaulted to when we move down
        },
        {
            { "Start Game", eFixedCommand::COMMAND_PROMPT_NEXT_MENU },
            {"Back", eFixedCommand::COMMAND_PROMPT_BACK}
        },
    }
)
{
    // Note that this is in the constructor, not enter(). While there are
    // initial bot and player values, these SHOULD persist between menu changes
    // during runtime because it's annoying to have the values changed away
    // from what the player chose.
    m_iBotCount = MAX_BOT_COUNT;
    m_iPlayerCount = INPUT_HANDLER->getJoystickCount();
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
        GAME_MANAGER->initializeNewGame(m_iPlayerCount, m_iBotCount, 10);
        nextMenu(GameMenu::getInstance());
        break;
    }
}

// Back returns to the mainmenu screen
void PregameMenu::back()
{
    nextMenu(MainMenu::getInstance());
}

void PregameMenu::moveCursor(eFixedCommand direction)
{
    int maxPlayerCount;
    switch (getCurrentPromptCommand())
    {
        case eFixedCommand::COMMAND_PROMPT_SELECT: // Player
            maxPlayerCount = FuncUtils::max(MIN_PLAYER_COUNT, INPUT_HANDLER->getJoystickCount());
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iPlayerCount = FuncUtils::moduloDecrease(m_iPlayerCount, 1, MIN_PLAYER_COUNT, maxPlayerCount);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iPlayerCount = FuncUtils::moduloIncrease(m_iPlayerCount, 1, MIN_PLAYER_COUNT, maxPlayerCount);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iPlayerCount << endl;
                break;
            }
            break;
        case eFixedCommand::COMMAND_PROMPT_SELECT_2: // Bot
            switch (direction)
            {
            case COMMAND_PROMPT_LEFT:
                m_iBotCount = FuncUtils::moduloDecrease(m_iBotCount, 1, MIN_BOT_COUNT, MAX_BOT_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iBotCount << endl;
                break;
            case COMMAND_PROMPT_RIGHT:
                m_iBotCount = FuncUtils::moduloIncrease(m_iBotCount, 1, MIN_BOT_COUNT, MAX_BOT_COUNT);
                SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
                cout << "\t" << m_iBotCount << endl;
                break;
            }
            break;
    }

    PromptMenu::moveCursor(direction);
}


