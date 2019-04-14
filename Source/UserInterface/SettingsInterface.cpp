#include "UserInterface/SettingsInterface.h"
#include "GameManager.h"
#include "Menus/SettingsMenu.h"

// Singleton instance
SettingsInterface* SettingsInterface::m_pInstance = nullptr;

SettingsInterface::SettingsInterface() : PromptInterface(
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

SettingsInterface* SettingsInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new SettingsInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void SettingsInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void SettingsInterface::reinitialize(float gameTime)
{
}

void SettingsInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_RULE_MENU);
}

