#include "UserInterface/PregameInterface.h"
#include "GameManager.h"

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
    renderNumberOfPlayer();
    renderNumberOfBot();
    renderOption();
}

void PregameInterface::renderNumberOfPlayer() {
    renderImage(IMAGE_NUMBER_OF_PLAYER, m_vComponentCoordinates[PLAYER_COUNT].first, m_vComponentCoordinates[PLAYER_COUNT].second, 1.0f);
    // get the player count and render number
}

void PregameInterface::renderNumberOfBot() {
    renderImage(IMAGE_NUMBER_OF_BOT, m_vComponentCoordinates[BOT_COUNT].first, m_vComponentCoordinates[BOT_COUNT].second, 1.0f);
    // get the bot count and render number
}

void PregameInterface::renderOption() {
    renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
    renderImage(IMAGE_START_1, m_vComponentCoordinates[START_GAME].first, m_vComponentCoordinates[START_GAME].second, 1.0f);
}
