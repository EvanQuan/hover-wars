#include "UserInterface/PauseInterface.h"
#include "GameManager.h"
#include "Menus/PauseMenu.h"

// Singleton instance
PauseInterface* PauseInterface::m_pInstance = nullptr;

PauseInterface::PauseInterface() : MenuInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.4f, 0.6f},
        // 2 Prompt 2
        {0.4f, 0.4f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.0f, 0.0f},
        // 2 Prompt 2
        {0.0f, 0.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

PauseInterface* PauseInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PauseInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
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
    renderImage(IMAGE_BACKGROUND_PAUSE_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderOption();
}

void PauseInterface::renderOption() {
    PauseMenu* m = (PauseMenu*)PauseMenu::getInstance();
    string option = m->getCurrentPrompt();

    if (option == CONTINUE_OPTION)
    {   
        renderImage(IMAGE_RESUME_2, m_vComponentCoordinates[CONTINUE].first, m_vComponentCoordinates[CONTINUE].second, 1.0f);
        renderImage(IMAGE_END_GAME_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
    }
    else if (option == END_GAME_OPTION)
    {
        renderImage(IMAGE_RESUME_1, m_vComponentCoordinates[CONTINUE].first, m_vComponentCoordinates[CONTINUE].second, 1.0f);
        renderImage(IMAGE_END_GAME_2, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
    }
    else {
        renderImage(IMAGE_RESUME_1, m_vComponentCoordinates[CONTINUE].first, m_vComponentCoordinates[CONTINUE].second, 1.0f);
        renderImage(IMAGE_END_GAME_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].second, 1.0f);
    }

}
