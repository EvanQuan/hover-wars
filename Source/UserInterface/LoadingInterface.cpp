#include "UserInterface/LoadingInterface.h"
#include "GameManager.h"
#include "Menus/LoadingMenu.h"

// Singleton instance
LoadingInterface* LoadingInterface::m_pInstance = nullptr;

/*
    Signifies the game is loading.
*/
LoadingInterface::LoadingInterface() : MenuInterface(
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
    GAME_MANAGER->addInterface(this);
}

LoadingInterface* LoadingInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new LoadingInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void LoadingInterface::update(float fSecondsSinceLastUpdate)
{
}

void LoadingInterface::reinitialize(float gameTime)
{
}

void LoadingInterface::render()
{
}
