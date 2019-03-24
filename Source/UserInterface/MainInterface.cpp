#include "UserInterface/MainInterface.h"

// Singleton instance
MainInterface* MainInterface::m_pInstance = nullptr;

MainInterface::MainInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
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

}

MainInterface* MainInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new MainInterface(iWidth, iHeight);
    }
    return m_pInstance;
}


void MainInterface::update(float fSecondsSinceLastUpdate)
{
}

void MainInterface::reinitialize(float gameTime)
{
}

void MainInterface::render()
{
}
