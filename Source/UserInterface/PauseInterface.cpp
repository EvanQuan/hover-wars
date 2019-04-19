#include "UserInterface/PauseInterface.h"
#include "GameManager.h"
#include "Menus/PauseMenu.h"

// Singleton instance
PauseInterface* PauseInterface::m_pInstance = nullptr;

PauseInterface::PauseInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Prompt 1
        {0.4f, 0.5f},
        // 2 Prompt 2
        {0.4f, 0.4f},
        // 3 Pauser
        {0.5f, 0.75f},

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
        // 3 Pauser
        {-230.0f, 0.0f},
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


void PauseInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void PauseInterface::reinitialize(float gameTime)
{
}

void PauseInterface::renderOverride()
{
    renderBackgroundImage(IMAGE_BACKGROUND_PAUSE_MENU);
    renderPauser();
    renderOption();
}

void PauseInterface::renderOption() {
    PauseMenu* m = (PauseMenu*)PauseMenu::getInstance();
    string option = m->getCurrentPrompt();

    renderImage(option == CONTINUE_OPTION ? IMAGE_RESUME_2 : IMAGE_RESUME_1, CONTINUE);
    renderImage(option == END_GAME_OPTION ? IMAGE_END_GAME_2 : IMAGE_END_GAME_1, MAIN_MENU);

}

void PauseInterface::renderPauser()
{
    PauseMenu* m = (PauseMenu*)PauseMenu::getInstance();
    eHovercraft pauser = m->getPauser();
    renderText(m_eHovercraftToString.at(pauser) + " paused the game",
        PAUSER,
        COLOR_WHITE);
}
