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
    GAME_MANAGER->addInterface(this);
}

PregameInterface* PregameInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PregameInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
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
    PregameMenu* m = (PregameMenu*)PregameMenu::getInstance();
    // If in team players, the number of players if forced to 4
    switch(m->getGameMode() == GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS ? MAX_PLAYER_COUNT : i_NumOfPlayer) {
        case 1:
            renderImage(IMAGE_MENU_1, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
        case 2:
            renderImage(IMAGE_MENU_2, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
        case 3:
            renderImage(IMAGE_MENU_3, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
        case 4:
            renderImage(IMAGE_MENU_4, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
    }
}

void PregameInterface::renderNumberOfBot(int i_NumOfBot) {
    // get the bot count and render number
    switch(i_NumOfBot) {
        case 0:
            renderImage(IMAGE_MENU_0, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 1:
            renderImage(IMAGE_MENU_1, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 2:
            renderImage(IMAGE_MENU_2, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 3:
            renderImage(IMAGE_MENU_3, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 4:
            renderImage(IMAGE_MENU_4, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
    }
}

void PregameInterface::renderGameTime(int i_GameTime) {
    // get the player count and render number
    switch(i_GameTime) {
        case 60:
            renderImage(IMAGE_1_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 120:
            renderImage(IMAGE_2_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 180:
            renderImage(IMAGE_3_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 240:
            renderImage(IMAGE_4_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 300:
            renderImage(IMAGE_5_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 360:
            renderImage(IMAGE_6_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 420:
            renderImage(IMAGE_7_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 480:
            renderImage(IMAGE_8_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 540:
            renderImage(IMAGE_9_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 600:
            renderImage(IMAGE_10_2, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
    }
        renderImage(IMAGE_MIN, m_vComponentCoordinates[GAME_TIME_NUMBER].first + 50, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
}

void PregameInterface::renderGameMode(eGameMode m_eGameMode) {
    switch (m_eGameMode) {
        case GAMEMODE_TEAM_BOTS_VS_SOLO_PLAYERS:
            renderImage(IMAGE_BOT_TEAM, m_vComponentCoordinates[GAMEMODE_TYPE].first, m_vComponentCoordinates[GAMEMODE_TYPE].second, 1.0f);
            break;
        case GAMEMODE_TEAMS_BOTS_VS_PLAYERS:
            renderImage(IMAGE_BOT_VS_PLAYERS, m_vComponentCoordinates[GAMEMODE_TYPE].first, m_vComponentCoordinates[GAMEMODE_TYPE].second, 1.0f);
            break;
        case GAMEMODE_FREE_FOR_ALL:
            renderImage(IMAGE_FREE_FOR_ALL, m_vComponentCoordinates[GAMEMODE_TYPE].first, m_vComponentCoordinates[GAMEMODE_TYPE].second, 1.0f);
            break;
        case GAMEMODE_TEAMS_PLAYERS1_VS_PLAYERS2_VS_BOTS:
            renderImage(IMAGE_PLAYER_TEAMS, m_vComponentCoordinates[GAMEMODE_TYPE].first, m_vComponentCoordinates[GAMEMODE_TYPE].second, 1.0f);
            break;

    }
}

void PregameInterface::renderScoreLoss(bool scoreLossEnabled)
{
    renderImage(scoreLossEnabled ? IMAGE_ENABLED : IMAGE_DISABLED, m_vComponentCoordinates[SCORELOSS_TYPE].first, m_vComponentCoordinates[SCORELOSS_TYPE].second, 1.0f);
}

void PregameInterface::renderMap(unsigned int mapNumber)
{
    switch (mapNumber)
    {
    case 0:
        renderImage(IMAGE_MENU_1,
            m_vComponentCoordinates[MAP_NUMBER].first,
            m_vComponentCoordinates[MAP_NUMBER].second,
            1.0f);
        break;
    case 1:
        renderImage(IMAGE_MENU_2,
            m_vComponentCoordinates[MAP_NUMBER].first,
            m_vComponentCoordinates[MAP_NUMBER].second,
            1.0f);
        break;
    }
}

void PregameInterface::renderBotDifficulty(eBotDifficulty m_eBotDifficulty) {
    switch (m_eBotDifficulty) {\
        case DIFFICULTY_EASY:
            renderImage(IMAGE_EASY,
                m_vComponentCoordinates[BOT_DIFFICULTY_VALUE].first,
                m_vComponentCoordinates[BOT_DIFFICULTY_VALUE].second,
                1.0f);
            break;
        case DIFFICULTY_MEDIUM:
            renderImage(IMAGE_MEDIUM,
                m_vComponentCoordinates[BOT_DIFFICULTY_VALUE].first,
                m_vComponentCoordinates[BOT_DIFFICULTY_VALUE].second,
                1.0f);
            break;
        case DIFFICULTY_HARD:
            renderImage(IMAGE_HARD,
                m_vComponentCoordinates[BOT_DIFFICULTY_VALUE].first,
                m_vComponentCoordinates[BOT_DIFFICULTY_VALUE].second,
                1.0f);
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
        m_vComponentCoordinates[PLAYER_COUNT].first,
        m_vComponentCoordinates[PLAYER_COUNT].second,
        1.0f);
    renderImage(option == BOT_COUNT_OPTION ?
        IMAGE_NUMBER_OF_BOT_2 : IMAGE_NUMBER_OF_BOT_1,
        m_vComponentCoordinates[BOT_COUNT].first,
        m_vComponentCoordinates[BOT_COUNT].second,
        1.0f);
    renderImage(option == BOT_DIFFICULTY_OPTION ?
        IMAGE_BOT_DIFFICULTY_2 : IMAGE_BOT_DIFFICULTY_1,
        m_vComponentCoordinates[BOT_DIFFICULTY].first,
        m_vComponentCoordinates[BOT_DIFFICULTY].second,
        1.0f);
    renderImage(option == GAME_TIME_OPTION ?
        IMAGE_GAME_TIME_2 : IMAGE_GAME_TIME_1,
        m_vComponentCoordinates[GAME_TIME].first,
        m_vComponentCoordinates[GAME_TIME].second,
        1.0f);
    renderImage(option == GAMEMODE_OPTION ?
        IMAGE_GAMEMODE_2 : IMAGE_GAMEMODE_1,
        m_vComponentCoordinates[GAMEMODE_LABEL].first,
        m_vComponentCoordinates[GAMEMODE_LABEL].second,
        1.0f);
    renderImage(option == SCORELOSS_OPTION ?
        IMAGE_SCORE_LOSS_2 : IMAGE_SCORE_LOSS_1,
        m_vComponentCoordinates[SCORELOSS_LABEL].first,
        m_vComponentCoordinates[SCORELOSS_LABEL].second,
        1.0f);
    renderImage(option == MAP_OPTION ?
        IMAGE_MAP_2 : IMAGE_MAP_1,
        m_vComponentCoordinates[MAP_LABEL].first,
        m_vComponentCoordinates[MAP_LABEL].second,
        1.0f);
    renderImage(option == BACK_OPTION ?
        IMAGE_MAIN_MENU_BUTTON_2 : IMAGE_MAIN_MENU_BUTTON_1,
        m_vComponentCoordinates[MAIN_MENU].first,
        m_vComponentCoordinates[MAIN_MENU].second,
        1.0f);
    renderImage(option == START_GAME_OPTION ?
        IMAGE_START_2 : IMAGE_START_1,
        m_vComponentCoordinates[START_GAME].first,
        m_vComponentCoordinates[START_GAME].second,
        1.0f);
}
