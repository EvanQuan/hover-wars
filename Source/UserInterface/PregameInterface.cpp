#include "UserInterface/PregameInterface.h"
#include "GameManager.h"
#include "Menus/PregameMenu.h"

// Singleton instance
PregameInterface* PregameInterface::m_pInstance = nullptr;

PregameInterface::PregameInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},

        // 1 Player count
        {0.2f, 0.8f},
        // 2 Player count number
        {0.6f, 0.8f},

        // 3 Bot count
        {0.2f, 0.7f},
        // 4 Bot count number
        {0.6f, 0.7f},

        // 5 Bot difficulty
        {0.2f, 0.6f},
        // 6 Bot difficulty value
        {0.6f, 0.6f},

        // 7 Game time
        {0.2f, 0.5f},
        // 8 Game time number
        {0.6f, 0.5f},

        // 9 Game mode label
        {0.2f, 0.4f},
        // 10 Game mode type
        {0.6f, 0.4f},

        // 11 Score loss label
        {0.2f, 0.3f},
        // 12 Score loss type
        {0.6f, 0.3f},

        // 11 Map label
        {0.2f, 0.2f},
        // 12 Map type
        {0.6f, 0.2f},

        // 13 Main menu
        {0.2f, 0.1f},

        // 14 Start game
        {0.5f, 0.1f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Player count
        {0.0f, 0.0f},
        // 2 Player count number
        {0.0f, 0.0f},
        // 3 Bot count
        {0.0f, 0.0f},
        // 4 Bot count number
        {0.0f, 0.0f},
        // Bot difficutly
        {0.0f, 0.0f},
        // Bot difficulty value
        {0.0f, 0.0f},
        // 5 Game time
        {0.0f, 0.0f},
        // 6 Game time number
        {0.0f, 0.0f},

        // 7 Game mode label
        {0.0f, 0.0f},
        // 8 Game mode type
        {0.0f, 0.0f},

        // 9 Score loss label
        {0.0f, 0.0f},
        // 10 Score loss type
        {0.0f, 0.0f},

        // 11 Map label
        {0.0f, 0.0f},
        // 12 Map type
        {0.0f, 0.0f},

        // 13 Main menu
        {0.0f, 0.0f},
        // 14 Start game
        {0.0f, 0.0f},
    }
)
{
}

PregameInterface* PregameInterface::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PregameInterface();
    }
    m_pInstance->updateWidthAndHeight(GAME_MANAGER->getWidth(), GAME_MANAGER->getHeight());
    return m_pInstance;
}


void PregameInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void PregameInterface::reinitialize(float gameTime)
{
}

void PregameInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_BACKGROUND_PRE_MENU);
    renderOption();
}

void PregameInterface::renderNumberOfPlayer(int i_NumOfPlayer) {
    // get the player count and render number
    PregameMenu* m = PregameMenu::getInstance();
    // If in team players, the number of players if forced to 4
    renderImage(digitToImage(m->getGameMode() == GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS ?
           MAX_PLAYER_COUNT : i_NumOfPlayer), PLAYER_COUNT_NUMBER);
}

void PregameInterface::renderNumberOfBot(int i_NumOfBot) {
    renderImage(digitToImage(i_NumOfBot), BOT_COUNT_NUMBER);
}

void PregameInterface::renderGameTime(int i_GameTime) {
    renderImage(digitToImage(i_GameTime / SECONDS_PER_MINUTE), GAME_TIME_NUMBER);
    renderImage(IMAGE_MIN, GAME_TIME_NUMBER, 50, 0);
}

void PregameInterface::renderGameMode(eGameMode m_eGameMode) {
    switch (m_eGameMode) {
        case GAMEMODE_TEAM_BOTS_VS_SOLO_PLAYERS:
            renderImage(IMAGE_BOT_TEAM_2, GAMEMODE_TYPE);
            break;
        case GAMEMODE_TEAMS_BOTS_VS_PLAYERS:
            renderImage(IMAGE_BOTS_VS_PLAYERS, GAMEMODE_TYPE);
            break;
        case GAMEMODE_FREE_FOR_ALL:
            renderImage(IMAGE_FREE_FOR_ALL, GAMEMODE_TYPE);
            break;
        case GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS:
            renderImage(IMAGE_PLAYER_TEAMS, GAMEMODE_TYPE);
            break;

    }
}

void PregameInterface::renderScoreLoss(bool scoreLossEnabled)
{
    renderImage(scoreLossEnabled ? IMAGE_ENABLED : IMAGE_DISABLED, SCORELOSS_TYPE);
}

void PregameInterface::renderMap(unsigned int mapNumber)
{
    // Add 1 as we index from 1 here
    renderImage(digitToImage(mapNumber + 1), MAP_NUMBER);

}

void PregameInterface::renderBotDifficulty(eBotDifficulty m_eBotDifficulty) {
    switch (m_eBotDifficulty) {
        case DIFFICULTY_EASY:
            renderImage(IMAGE_EASY, BOT_DIFFICULTY_VALUE);
            break;
        case DIFFICULTY_MEDIUM:
            renderImage(IMAGE_MEDIUM, BOT_DIFFICULTY_VALUE);
            break;
        case DIFFICULTY_HARD:
            renderImage(IMAGE_HARD, BOT_DIFFICULTY_VALUE);
            break;
    }
}

void PregameInterface::renderOption() {
    PregameMenu* m = PregameMenu::getInstance();
    string option = m->getCurrentPrompt();
    renderNumberOfPlayer(m->getPlayerCount());
    renderNumberOfBot(m->getBotCount());
    renderGameTime(m->getGameTime());
    renderGameMode(m->getGameMode());
    renderScoreLoss(m->hasScoreLossEnabled());
    renderMap(m->getMapNumber());
    renderBotDifficulty(m->getBotDifficulty());

    renderImage(option == PLAY_COUNT_OPTION ?
        IMAGE_NUMBER_OF_PLAYER_2 : IMAGE_NUMBER_OF_PLAYER_1,
        PLAYER_COUNT);
    renderImage(option == BOT_COUNT_OPTION ?
        IMAGE_NUMBER_OF_BOT_2 : IMAGE_NUMBER_OF_BOT_1,
        BOT_COUNT);
    renderImage(option == BOT_DIFFICULTY_OPTION ?
        IMAGE_BOT_DIFFICULTY_2 : IMAGE_BOT_DIFFICULTY_1,
        BOT_DIFFICULTY);
    renderImage(option == GAME_TIME_OPTION ?
        IMAGE_GAME_TIME_2 : IMAGE_GAME_TIME_1,
        GAME_TIME);
    renderImage(option == GAMEMODE_OPTION ?
        IMAGE_GAMEMODE_2 : IMAGE_GAMEMODE_1,
        GAMEMODE_LABEL);
    renderImage(option == SCORELOSS_OPTION ?
        IMAGE_SCORE_LOSS_2 : IMAGE_SCORE_LOSS_1,
        SCORELOSS_LABEL);
    renderImage(option == MAP_OPTION ?
        IMAGE_MAP_2 : IMAGE_MAP_1,
        MAP_LABEL);
    renderImage(option == BACK_OPTION ?
        IMAGE_MAIN_MENU_BUTTON_2 : IMAGE_MAIN_MENU_BUTTON_1,
        MAIN_MENU);
    renderImage(option == START_GAME_OPTION ?
        IMAGE_START_2 : IMAGE_START_1,
        START_GAME);
}
