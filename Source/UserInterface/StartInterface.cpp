#include "UserInterface/StartInterface.h"
#include "GameManager.h"
#include "Menus/StartMenu.h"

// Singleton instance
StartInterface* StartInterface::m_pInstance = nullptr;

StartInterface::StartInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.25f, 0.7f},
        // 2 Prompt 1
        {0.38f, 0.2f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.0f, 0.0f},
        // 2 Prompt 1
        {0.0f, 0.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

StartInterface* StartInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new StartInterface(iWidth, iHeight);
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void StartInterface::update(float fSecondsSinceLastUpdate)
{
}

void StartInterface::reinitialize(float gameTime)
{
}

void StartInterface::render()
{
    renderImage(IMAGE_BACKGROUND, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderImage(IMAGE_TITLE, m_vComponentCoordinates[TITLE].first, m_vComponentCoordinates[TITLE].second, 1.0f);
    renderOption(); 
}

void StartInterface::renderOption() {
    StartMenu* m = (StartMenu* )StartMenu::getInstance();
    string option = m->getCurrentPrompt();
    if (option == INSERT_COIN_OPTION) {
        renderImage(IMAGE_INSERT_COIN_2, m_vComponentCoordinates[INSERT_COIN].first, m_vComponentCoordinates[INSERT_COIN].second, 1.0f);
    }
    else {
        renderImage(IMAGE_INSERT_COIN_1, m_vComponentCoordinates[INSERT_COIN].first, m_vComponentCoordinates[INSERT_COIN].second, 1.0f);
    }
}
