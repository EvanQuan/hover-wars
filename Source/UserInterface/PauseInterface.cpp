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
        // 2 Prompt 2
        {0.0f, 0.0f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.47f, 0.5f},
        // 2 Prompt 2
        {0.47f, 0.6f},
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
    renderImage(IMAGE_BACKGROUND, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].first, 1.0f);
    renderOption(0);
}

void PauseInterface::renderOption(int choice) {
    if (choice == 1)
    {   // cursor in 'new game'
        renderImage(IMAGE_RESUME_1, m_vComponentCoordinates[CONTINUE].first, m_vComponentCoordinates[CONTINUE].first, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].first, 1.0f);
    }
    else if (choice == 2) {
        renderImage(IMAGE_RESUME_2, m_vComponentCoordinates[CONTINUE].first, m_vComponentCoordinates[CONTINUE].first, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_1, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].first, 1.0f);
    }
    else {
        renderImage(IMAGE_RESUME_1, m_vComponentCoordinates[CONTINUE].first, m_vComponentCoordinates[CONTINUE].first, 1.0f);
        renderImage(IMAGE_MAIN_MENU_BUTTON_2, m_vComponentCoordinates[MAIN_MENU].first, m_vComponentCoordinates[MAIN_MENU].first, 1.0f);
    }

}
