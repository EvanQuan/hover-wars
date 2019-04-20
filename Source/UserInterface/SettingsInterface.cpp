#include "UserInterface/SettingsInterface.h"
#include "GameManager.h"
#include "Menus/SettingsMenu.h"

// Singleton instance
SettingsInterface* SettingsInterface::m_pInstance = nullptr;

SettingsInterface::SettingsInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 title
        {0.37f, 0.8f},
        // 1 music label
        {0.2f, 0.5f},
        // 2 music setting
        {0.6f, 0.5f},
        // 3 back
        {0.37f, 0.3f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0
        {0.0f, 0.0f},
        // 1 
        {0.0f, 0.0f},
        // 2 
        {0.0f, 0.0f},
        // 3 
        {0.0f, 0.0f},
    }
)
{
}

SettingsInterface* SettingsInterface::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new SettingsInterface();
    }
    m_pInstance->updateWidthAndHeight(GAME_MANAGER->getWidth(), GAME_MANAGER->getHeight());
    return m_pInstance;
}


void SettingsInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void SettingsInterface::reinitialize(float gameTime)
{
}

void SettingsInterface::renderOverride()
{
    SettingsMenu* menu = SettingsMenu::getInstance();
    string prompt = menu->getCurrentPrompt();
    renderBackgroundImage(IMAGE_BACKGROUND_MAIN_MENU);
    renderTitle();
    renderMusic(prompt == "Music", menu->hasMusicEnabled());
    renderBack(prompt == "Back");
}

void SettingsInterface::renderTitle()
{
    renderImage(IMAGE_SETTINGS_1, TITLE);
}

void SettingsInterface::renderMusic(bool selected, bool musicEnabled)
{
    renderImage(selected ? IMAGE_MUSIC_2 : IMAGE_MUSIC_1, MUSIC_LABEL);
    renderImage(musicEnabled ? IMAGE_ENABLED : IMAGE_DISABLED, MUSIC_SETTING);
}

void SettingsInterface::renderBack(bool selected)
{
    renderImage(selected ? IMAGE_MAIN_MENU_BUTTON_2 : IMAGE_MAIN_MENU_BUTTON_1, BACK);
}

