#include "UserInterface/EndgameInterface.h"
#include "GameManager.h"
#include "Menus/EndgameMenu.h"

// Singleton instance
EndgameInterface* EndgameInterface::m_pInstance = nullptr;

EndgameInterface::EndgameInterface() : PromptInterface(
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
    GAME_MANAGER->addInterface(this);
}

EndgameInterface* EndgameInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new EndgameInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void EndgameInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void EndgameInterface::reinitialize(float gameTime)
{
}

void EndgameInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_RULE_MENU);
    renderText(GAME_VERSION,
        m_vComponentCoordinates[VERSION].first,
        m_vComponentCoordinates[VERSION].second,
        1.0,
        COLOR_WHITE);
}

