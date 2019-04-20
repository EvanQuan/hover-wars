#include "UserInterface/RulesInterface.h"
#include "GameManager.h"
#include "Menus/RulesMenu.h"

// Singleton instance
RulesInterface* RulesInterface::m_pInstance = nullptr;

RulesInterface::RulesInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Version
        {0.0f, 0.0f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Version
        {10.0f, 15.0f},
    }
)
{
}

RulesInterface* RulesInterface::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new RulesInterface();
    }
    m_pInstance->updateWidthAndHeight(GAME_MANAGER->getWidth(), GAME_MANAGER->getHeight());
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
    renderBackgroundImage(IMAGE_RULE_MENU);
    renderText(GAME_VERSION, VERSION, COLOR_WHITE);
}

