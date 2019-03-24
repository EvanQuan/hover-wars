#include "UserInterface/PauseInterface.h"
#include "GameManager.h"

// Singleton instance
PauseInterface* PauseInterface::m_pInstance = nullptr;

PauseInterface::PauseInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
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

PauseInterface* PauseInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PauseInterface(iWidth, iHeight);
    }
    return m_pInstance;
}


void PauseInterface::update(float fSecondsSinceLastUpdate)
{
}

void PauseInterface::reinitialize(float gameTime)
{
}

void PauseInterface::render()
{
}
