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
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
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

void RulesInterface::renderOverride()
{
    renderImage(IMAGE_RULE_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
}

