#include "UserInterface/MainInterface.h"
#include "GameManager.h"
#include "Menus/MainMenu.h"

// Singleton instance
MainInterface* MainInterface::m_pInstance = nullptr;

/*

    New Game

      Quit

*/
MainInterface::MainInterface() : MenuInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.25f, 0.7f},
        // 2 New Game
        {0.37f, 0.55f},
        // 3 Game Rule
        {0.37f, 0.40f},
        // 4 Controller
        {0.37f, 0.25f},
        // 5 Exit
        {0.37f, 0.10f},
    },
    // Translating
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
        // 4
        {0.0f, 0.0f},
        // 5
        {0.0f, 0.0f},
    }
)
{
}

MainInterface* MainInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new MainInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
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
    renderImage(IMAGE_BACKGROUND_MAIN_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderImage(IMAGE_TITLE, m_vComponentCoordinates[TITLE].first, m_vComponentCoordinates[TITLE].second, 1.0f);

    renderOption();
}

void MainInterface::renderOption() {
    MainMenu* m = (MainMenu*) MainMenu::getInstance();
    string option = m->getCurrentPrompt();

    if (option == NEW_GAME_OPTION)
    {
        renderImage(IMAGE_NEW_GAME_2, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].second, 1.0f);
        renderImage(IMAGE_GAME_RULE_1, m_vComponentCoordinates[RULE].first, m_vComponentCoordinates[RULE].second, 1.0f);
        renderImage(IMAGE_CONTROLLER_1, m_vComponentCoordinates[CONTROLLER].first, m_vComponentCoordinates[CONTROLLER].second, 1.0f);
        renderImage(IMAGE_EXIT_1, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].second, 1.0f);
    }
    else if (option == GAME_RULE) {
        renderImage(IMAGE_NEW_GAME_1, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].second, 1.0f);
        renderImage(IMAGE_GAME_RULE_2, m_vComponentCoordinates[RULE].first, m_vComponentCoordinates[RULE].second, 1.0f);
        renderImage(IMAGE_CONTROLLER_1, m_vComponentCoordinates[CONTROLLER].first, m_vComponentCoordinates[CONTROLLER].second, 1.0f);
        renderImage(IMAGE_EXIT_1, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].second, 1.0f);
    }
    else if (option == GAME_CONTROLLER) {
        renderImage(IMAGE_NEW_GAME_1, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].second, 1.0f);
        renderImage(IMAGE_GAME_RULE_1, m_vComponentCoordinates[RULE].first, m_vComponentCoordinates[RULE].second, 1.0f);
        renderImage(IMAGE_CONTROLLER_2, m_vComponentCoordinates[CONTROLLER].first, m_vComponentCoordinates[CONTROLLER].second, 1.0f);
        renderImage(IMAGE_EXIT_1, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].second, 1.0f);
    }
    else if (option == QUIT_OPTION) {
        renderImage(IMAGE_NEW_GAME_1, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].second, 1.0f);
        renderImage(IMAGE_GAME_RULE_1, m_vComponentCoordinates[RULE].first, m_vComponentCoordinates[RULE].second, 1.0f);
        renderImage(IMAGE_CONTROLLER_1, m_vComponentCoordinates[CONTROLLER].first, m_vComponentCoordinates[CONTROLLER].second, 1.0f);
        renderImage(IMAGE_EXIT_2, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].second, 1.0f);
    }
    else {
        renderImage(IMAGE_NEW_GAME_1, m_vComponentCoordinates[NEW_GAME].first, m_vComponentCoordinates[NEW_GAME].second, 1.0f);
        renderImage(IMAGE_GAME_RULE_1, m_vComponentCoordinates[RULE].first, m_vComponentCoordinates[RULE].second, 1.0f);
        renderImage(IMAGE_CONTROLLER_1, m_vComponentCoordinates[CONTROLLER].first, m_vComponentCoordinates[CONTROLLER].second, 1.0f);
        renderImage(IMAGE_EXIT_1, m_vComponentCoordinates[EXIT].first, m_vComponentCoordinates[EXIT].second, 1.0f);
    }
}
