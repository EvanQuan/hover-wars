#include "UserInterface/ControlsInterface.h"
#include "GameManager.h"
#include "Menus/ControlsMenu.h"

// Singleton instance
ControlsInterface* ControlsInterface::m_pInstance = nullptr;

ControlsInterface::ControlsInterface() : MenuInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.0f, 0.0f},
        // 2 Prompt 1
        {0.0f, 0.0f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.47f, 0.9f},
        // 2 Prompt 1
        {0.47f, 0.2f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

ControlsInterface* ControlsInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new ControlsInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void ControlsInterface::update(float fSecondsSinceLastUpdate)
{
}

void ControlsInterface::reinitialize(float gameTime)
{
}

void ControlsInterface::render()
{
    renderImage(IMAGE_BACKGROUND, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderImage(IMAGE_TITLE, m_vComponentCoordinates[TITLE].first, m_vComponentCoordinates[TITLE].second, 1.0f);
    renderOption();
}

void ControlsInterface::renderOption() {
    ControlsMenu* m = (ControlsMenu* )ControlsMenu::getInstance();
    string option = m->getCurrentPrompt();
    if (option == INSERT_COIN_OPTION) {
        renderImage(IMAGE_INSERT_COIN_2, m_vComponentCoordinates[INSERT_COIN].first, m_vComponentCoordinates[INSERT_COIN].second, 1.0f);
    }
    else {
        renderImage(IMAGE_INSERT_COIN_1, m_vComponentCoordinates[INSERT_COIN].first, m_vComponentCoordinates[INSERT_COIN].second, 1.0f);
    }
}
