#include "UserInterface/PregameInterface.h"
#include "GameManager.h"
#include "Menus/PregameMenu.h"

// Singleton instance
PregameInterface* PregameInterface::m_pInstance = nullptr;

PregameInterface::PregameInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Player count
        {0.0f, 0.0f},
        // 2 Bot count
        {0.0f, 0.0f},
        // 3 Main menu
        {0.0f, 0.0f},
        // 4 Start game
        {0.0f, 0.0f},
        // 5 Player count number
        {0.0f, 0.0f},
        // 6 Bot count number
        {0.0f, 0.0f},
        // 7 Game time
        { 0.0f, 0.0f },
        // 8 Game time number
        { 0.0f, 0.0f },
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Player count
        {0.25f, 0.07f},
        // 2 Bot count
        {0.25f, 0.5f},
        // 3 Main menu
        {0.25f, 0.2f},
        // 4 Start game
        {0.75f, 0.2f},
        // 5 Player count number
        {0.5f, 0.7f},
        // 6 Bot count number
        {0.5f, 0.5f},
        // 7 Game time
        {0.25f, 0.3f},
        // 8 Game time number
        {0.5f, 0.3f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

PregameInterface* PregameInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PregameInterface(iWidth, iHeight);
    }
    return m_pInstance;
}


void PregameInterface::update(float fSecondsSinceLastUpdate)
{
}

void PregameInterface::reinitialize(float gameTime)
{
}

void PregameInterface::render()
{
    renderImage(IMAGE_BACKGROUND, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderOption();
}

void PregameInterface::renderNumberOfPlayer(int i_NumOfPlayer) {
    // get the player count and render number
    switch(i_NumOfPlayer) {
        case 1:
            renderImage(IMAGE_1_1, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
        case 2:
            renderImage(IMAGE_2_1, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
        case 3:
            renderImage(IMAGE_3_1, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
        case 4:
            renderImage(IMAGE_4_1, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].first, m_vComponentCoordinates[PLAYER_COUNT_NUMBER].second, 1.0f);
            break;
    }
}

void PregameInterface::renderNumberOfBot(int i_NumOfBot) {
    // get the bot count and render number
    switch(i_NumOfBot) {
        case 1:
            renderImage(IMAGE_1_1, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 2:
            renderImage(IMAGE_2_1, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 3:
            renderImage(IMAGE_3_1, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
        case 4:
            renderImage(IMAGE_4_1, m_vComponentCoordinates[BOT_COUNT_NUMBER].first, m_vComponentCoordinates[BOT_COUNT_NUMBER].second, 1.0f);
            break;
    }
}

void PregameInterface::renderGameTime(int i_GameTime) {
    // get the player count and render number
    switch(i_GameTime) {
        case 1:
            renderImage(IMAGE_1_MIN, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 2:
            renderImage(IMAGE_2_MIN, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 3:
            renderImage(IMAGE_3_MIN, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
        case 4:
            renderImage(IMAGE_4_MIN, m_vComponentCoordinates[GAME_TIME_NUMBER].first, m_vComponentCoordinates[GAME_TIME_NUMBER].second, 1.0f);
            break;
    }
}

void PregameInterface::renderOption() {
    PregameMenu* m = (PregameMenu*)PregameMenu::getInstance();
    string option = m->getCurrentPrompt();
    renderNumberOfPlayer(m->getPlayerCount());
    renderNumberOfBot(m->getBotCount());
    
    if (option == PLAY_COUNT_OPTION) {
        renderImage(IMAGE_NUMBER_OF_PLAYER_2, m_vComponentCoordinates[PLAYER_COUNT].first, m_vComponentCoordinates[PLAYER_COUNT].second, 1.0f);
        renderImage(IMAGE_NUMBER_OF_BOT_1, m_vComponentCoordinates[BOT_COUNT].first, m_vComponentCoordinates[BOT_COUNT].second, 1.0f);
        renderImage(IMAGE_GAME_TIME_1, m_vComponentCoordinates[GAME_TIME].first, m_vComponentCoordinates[GAME_TIME].second, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
        renderImage(IMAGE_START_1, m_vComponentCoordinates[START_GAME].first, m_vComponentCoordinates[START_GAME].second, 1.0f);
    }
    else if (option == BOT_COUNT_OPTION) {
        renderImage(IMAGE_NUMBER_OF_PLAYER_1, m_vComponentCoordinates[PLAYER_COUNT].first, m_vComponentCoordinates[PLAYER_COUNT].second, 1.0f);
        renderImage(IMAGE_NUMBER_OF_BOT_2, m_vComponentCoordinates[BOT_COUNT].first, m_vComponentCoordinates[BOT_COUNT].second, 1.0f);
        renderImage(IMAGE_GAME_TIME_1, m_vComponentCoordinates[GAME_TIME].first, m_vComponentCoordinates[GAME_TIME].second, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
        renderImage(IMAGE_START_1, m_vComponentCoordinates[START_GAME].first, m_vComponentCoordinates[START_GAME].second, 1.0f);
    }
    else if (option == GAME_TIME_OPTION) {
        renderImage(IMAGE_NUMBER_OF_PLAYER_1, m_vComponentCoordinates[PLAYER_COUNT].first, m_vComponentCoordinates[PLAYER_COUNT].second, 1.0f);
        renderImage(IMAGE_NUMBER_OF_BOT_2, m_vComponentCoordinates[BOT_COUNT].first, m_vComponentCoordinates[BOT_COUNT].second, 1.0f);
        renderImage(IMAGE_GAME_TIME_2, m_vComponentCoordinates[GAME_TIME].first, m_vComponentCoordinates[GAME_TIME].second, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
        renderImage(IMAGE_START_1, m_vComponentCoordinates[START_GAME].first, m_vComponentCoordinates[START_GAME].second, 1.0f);
    }
    else if (option == START_GAME_OPTION) {
        renderImage(IMAGE_NUMBER_OF_PLAYER_1, m_vComponentCoordinates[PLAYER_COUNT].first, m_vComponentCoordinates[PLAYER_COUNT].second, 1.0f);
        renderImage(IMAGE_NUMBER_OF_BOT_1, m_vComponentCoordinates[BOT_COUNT].first, m_vComponentCoordinates[BOT_COUNT].second, 1.0f);
        renderImage(IMAGE_GAME_TIME_1, m_vComponentCoordinates[GAME_TIME].first, m_vComponentCoordinates[GAME_TIME].second, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
        renderImage(IMAGE_START_2, m_vComponentCoordinates[START_GAME].first, m_vComponentCoordinates[START_GAME].second, 1.0f);
    }
    else if (option == BACK_OPTION) {
        renderImage(IMAGE_NUMBER_OF_PLAYER_1, m_vComponentCoordinates[PLAYER_COUNT].first, m_vComponentCoordinates[PLAYER_COUNT].second, 1.0f);
        renderImage(IMAGE_NUMBER_OF_BOT_1, m_vComponentCoordinates[BOT_COUNT].first, m_vComponentCoordinates[BOT_COUNT].second, 1.0f);
        renderImage(IMAGE_GAME_TIME_1, m_vComponentCoordinates[GAME_TIME].first, m_vComponentCoordinates[GAME_TIME].second, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_2, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
        renderImage(IMAGE_START_1, m_vComponentCoordinates[START_GAME].first, m_vComponentCoordinates[START_GAME].second, 1.0f);
    }
}
