#include "UserInterface/StartInterface.h"
#include "GameManager.h"

// Singleton instance
StartInterface* StartInterface::m_pInstance = nullptr;

StartInterface::StartInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.00f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 2 Cursor 1
        {0.0f, 0.0f},
        // 3 Prompt 2
        {0.0f, 0.0f},
        // 4 Cursor 2
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.00f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 2 Cursor 1
        {0.0f, 0.0f},
        // 3 Prompt 2
        {0.0f, 0.0f},
        // 4 Cursor 2
    }
)
{
    GAME_MANAGER->addInterface(this);
}

StartInterface* StartInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new StartInterface(iWidth, iHeight);
    }
    return m_pInstance;
}


void StartInterface::update(float fSecondsSinceLastUpdate)
{
}

void StartInterface::reinitialize(float gameTime)
{
}

void StartInterface::render()
{
}
