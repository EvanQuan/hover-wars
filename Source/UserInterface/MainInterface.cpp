#include "UserInterface/MainInterface.h"
#include "GameManager.h"
#include "Menus/MainMenu.h"

// Singleton instance
MainInterface* MainInterface::m_pInstance = nullptr;

MainInterface::MainInterface(int iWidth, int iHeight) : MenuInterface(iWidth, iHeight,
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.0f, 0.0f},
        // 2 Prompt 1
        {0.0f, 0.0f},
        // 3 Prompt 2
        {0.0f, 0.0f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.47f, 0.9f},
        // 2 Prompt 1
        {0.47f, 0.3f},
        // 3 Prompt 2
        {0.47f, 0.2f},
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
    renderImage(IMAGE_BACKGROUND, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].first, 1.0f);
    renderImage(IMAGE_TITLE, m_vComponentCoordinates[TITLE].first, m_vComponentCoordinates[TITLE].second, 1.0f);

    renderOption(0);
}

void MainInterface::renderOption(int choice) {
    MainMenu* m = (MainMenu*) MainMenu::getInstance();
    m->getCurrentPrompt();
    //string s = MainMenu::getCurrentPrompt();

    if (choice == 1)
    {   // cursor in 'new game'
        renderImage(IMAGE_NEW_GAME_1, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].first, 1.0f);
        renderImage(IMAGE_EXIT_1, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].first, 1.0f);
    }
    else if (choice == 2) {
        renderImage(IMAGE_NEW_GAME_2, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].first, 1.0f);
        renderImage(IMAGE_EXIT_1, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].first, 1.0f);
    }
    else {
        renderImage(IMAGE_NEW_GAME_1, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].first, 1.0f);
        renderImage(IMAGE_EXIT_2, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].first, 1.0f);
    }
    
}
