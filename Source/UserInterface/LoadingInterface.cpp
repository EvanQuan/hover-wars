#include "UserInterface/LoadingInterface.h"
#include "GameManager.h"
#include "Menus/LoadingMenu.h"

// Singleton instance
LoadingInterface* LoadingInterface::m_pInstance = nullptr;

/*
    Signifies the game is loading.
*/
LoadingInterface::LoadingInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},

    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.47f, 0.1f},
    }
)
{
}

LoadingInterface* LoadingInterface::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new LoadingInterface();
    }
    m_pInstance->updateWidthAndHeight(GAME_MANAGER->getWidth(), GAME_MANAGER->getHeight());
    return m_pInstance;
}


void LoadingInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void LoadingInterface::reinitialize(float gameTime)
{
}

void LoadingInterface::renderOverride()
{
}
