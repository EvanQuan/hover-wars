#include "UserInterface/PostgameInterface.h"
#include "GameManager.h"

// Singleton instance
PostgameInterface* PostgameInterface::m_pInstance = nullptr;

PostgameInterface::PostgameInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 2 Cursor 1
        {0.0f, 0.0f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 2 Cursor 1
        {0.0f, 0.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

PostgameInterface* PostgameInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PostgameInterface(iWidth, iHeight);
    }
    return m_pInstance;
}


void PostgameInterface::update(float fSecondsSinceLastUpdate)
{
}

void PostgameInterface::reinitialize(float gameTime)
{
}

void PostgameInterface::render()
{
}
