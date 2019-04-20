#include "UserInterface/MainInterface.h"
#include "GameManager.h"
#include "Menus/MainMenu.h"

// Singleton instance
MainInterface* MainInterface::m_pInstance = nullptr;

/*

    New Game

      Quit

*/
MainInterface::MainInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.27f, 0.75f},
        // 2 New Game
        {0.37f, 0.60f},
        // 3 Game Rule
        {0.37f, 0.50f},
        // 4 Controller
        {0.37f, 0.40f},
        // 5 Controller
        {0.37f, 0.30f},
        // 6 Exit
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
        // 6
        {0.0f, 0.0f},
    }
)
{
}

MainInterface* MainInterface::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new MainInterface();
    }
    m_pInstance->updateWidthAndHeight(GAME_MANAGER->getWidth(), GAME_MANAGER->getHeight());
    return m_pInstance;
}


void MainInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void MainInterface::reinitialize(float gameTime)
{
}

void MainInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_BACKGROUND_MAIN_MENU);
    renderImage(IMAGE_TITLE, TITLE);

    renderOption();
}

void MainInterface::renderOption() {
    MainMenu* m = (MainMenu*) MainMenu::getInstance();
    string option = m->getCurrentPrompt();

    renderImage(option == NEW_GAME_OPTION ? IMAGE_NEW_GAME_2 : IMAGE_NEW_GAME_1, NEW_GAME);
    renderImage(option == GAME_RULE ? IMAGE_GAME_RULE_2 : IMAGE_GAME_RULE_1, RULE);
    renderImage(option == GAME_CONTROLLER ? IMAGE_CONTROLLER_2 : IMAGE_CONTROLLER_1, CONTROLLER);
    renderImage(option == SETTINGS_OPTION ? IMAGE_SETTINGS_2 : IMAGE_SETTINGS_1, SETTINGS);
    renderImage(option == QUIT_OPTION ? IMAGE_EXIT_2 : IMAGE_EXIT_1, EXIT);
}
