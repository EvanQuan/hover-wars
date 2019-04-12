#include "UserInterface/AwardsInterface.h"
#include "GameManager.h"
#include "Menus/AwardsMenu.h"


// Singleton instance
AwardsInterface* AwardsInterface::m_pInstance = nullptr;

AwardsInterface::AwardsInterface() : MenuInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Title
        {0.2f, 0.7f},
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
}

AwardsInterface* AwardsInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new AwardsInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void AwardsInterface::updateOverride(float fSecondsSinceLastUpdate)
{
}

void AwardsInterface::reinitialize(float gameTime)
{
}

void AwardsInterface::renderOverride()
{
    renderImage(IMAGE_BACKGROUND_START_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    //renderImage(IMAGE_AWARDS_2, m_vComponentCoordinates[TITLE].first, m_vComponentCoordinates[TITLE].second, 1.0f);
    renderOption(); 
}

void AwardsInterface::renderOption() {

}

void AwardsInterface::renderAwards() {
    //vector<EndGameStat> endGameStats = GAME_STATS->getEndGameStats();

    //renderText();
}
