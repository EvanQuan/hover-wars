#include "UserInterface/ControlsInterface.h"
#include "GameManager.h"
#include "Menus/ControlsMenu.h"

// Singleton instance
ControlsInterface* ControlsInterface::m_pInstance = nullptr;

ControlsInterface::ControlsInterface() : PromptInterface(
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
}

ControlsInterface* ControlsInterface::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new ControlsInterface();
    }
    m_pInstance->updateWidthAndHeight(GAME_MANAGER->getWidth(), GAME_MANAGER->getHeight());
    return m_pInstance;
}


void ControlsInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void ControlsInterface::reinitialize(float gameTime)
{
}

void ControlsInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_CONTROLLER_MENU);
}
