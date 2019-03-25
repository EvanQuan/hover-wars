#include "UserInterface/MainInterface.h"
#include "GameManager.h"

// Singleton instance
MainInterface* MainInterface::m_pInstance = nullptr;

/*

    New Game

      Quit

*/
MainInterface::MainInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.00f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 2 Prompt 2
        {0.0f, 0.0f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.00f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 3 Prompt 2
        {0.0f, 0.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
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
    renderImage(IMAGE_BACKGROUND, m_vComponentCoordinates[COMPONENT_BACKGROUND].first, m_vComponentCoordinates[COMPONENT_BACKGROUND].first, 1.0f);
}

void MainInterface::renderOption(int choice) {
    if (choice == 1)
    {   // cursor in 'new game'
        //renderImage(IMAGE_NEW_GAME_2, , );
        //renderImage(IMAGE_EXIT_1)
    }
    else if (choice == 2) {
        //renderImage(IMAGE_NEW_GAME_1);
        //renderImage(IMAGE_EXIT_2)
    }
    
}
