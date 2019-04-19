#include "UserInterface/StartInterface.h"
#include "GameManager.h"
#include "Menus/StartMenu.h"

// seconds
#define BLINK_INTERVAL 0.5f

// Singleton instance
StartInterface* StartInterface::m_pInstance = nullptr;

StartInterface::StartInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.27f, 0.75f},
        // 2 Prompt 1
        {0.38f, 0.2f},
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
    }
)
{
    GAME_MANAGER->addInterface(this);

    m_bPromptVisible = true;
    m_fBlinkTime = BLINK_INTERVAL;
}

StartInterface* StartInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new StartInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void StartInterface::updateOverride(float fTimeInSeconds)
{
    m_fBlinkTime -= fTimeInSeconds;
    if (m_fBlinkTime < 0)
    {
        m_bPromptVisible = !m_bPromptVisible;
        m_fBlinkTime = BLINK_INTERVAL;
    }
}

void StartInterface::reinitialize(float gameTime)
{
}

void StartInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_BACKGROUND_START_MENU);
    renderImage(IMAGE_TITLE, TITLE);
    renderOption(); 
}

void StartInterface::renderOption() {
    StartMenu* m = static_cast<StartMenu*>(StartMenu::getInstance());
    // Since there is only 1 prompt, we don't care which prompt is selected.
    if (m_bPromptVisible)
    {
        renderImage(IMAGE_INSERT_COIN_2, INSERT_COIN);
    }
}
