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

ControlsInterface* ControlsInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new ControlsInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
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
    renderImage(IMAGE_CONTROLLER_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 2.0f);
}
