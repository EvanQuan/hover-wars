#include "UserInterface/RulesInterface.h"
#include "GameManager.h"
#include "Menus/RulesMenu.h"

// Singleton instance
RulesInterface* RulesInterface::m_pInstance = nullptr;

RulesInterface::RulesInterface() : MenuInterface(
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

RulesInterface* RulesInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new RulesInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void RulesInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void RulesInterface::reinitialize(float gameTime)
{
}

void RulesInterface::render()
{
    renderImage(IMAGE_BACKGROUND_MAIN_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderImage(IMAGE_TITLE, m_vComponentCoordinates[TITLE].first, m_vComponentCoordinates[TITLE].second, 1.0f);
    renderOption();
}

void RulesInterface::renderOption() {
    RulesMenu* m = (RulesMenu* )RulesMenu::getInstance();
    string option = m->getCurrentPrompt();
    if (option == INSERT_COIN_OPTION) {
        renderImage(IMAGE_INSERT_COIN_2, m_vComponentCoordinates[INSERT_COIN].first, m_vComponentCoordinates[INSERT_COIN].second, 1.0f);
    }
    else {
        renderImage(IMAGE_INSERT_COIN_1, m_vComponentCoordinates[INSERT_COIN].first, m_vComponentCoordinates[INSERT_COIN].second, 1.0f);
    }
}
