#include "UserInterface/PostgameInterface.h"
#include "GameManager.h"
#include "Menus/PostgameMenu.h"

// Singleton instance
PostgameInterface* PostgameInterface::m_pInstance = nullptr;

/*
    Placements                  Awards
    Player #    <score>         Award1 name <award1 points>
    Player #    <score>         Award1 description
    Bot    #    <score>         
    Player #    <score>         Award2 name <points>
    Bot    #    <score>         Award2 descrption


                        Main Menu
*/
PostgameInterface::PostgameInterface() : MenuInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Placement
        {0.0f, 0.0f},
        // 2 Awards
        {0.0f, 0.0f},

    },
    // Translating
    vector<pair<float, float>>
    {
        // 0 Background
        {0.0f, 0.0f},
        // 1 Placement
        {0.47f, 0.1f},
        // 2 Awards
        {0.0f, 0.0f},
    }
)
{
    GAME_MANAGER->addInterface(this);
}

PostgameInterface* PostgameInterface::getInstance(int iWidth, int iHeight)
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new PostgameInterface();
    }
    m_pInstance->updateWidthAndHeight(iWidth, iHeight);
    return m_pInstance;
}


void PostgameInterface::update(float fSecondsSinceLastUpdate)
{
}

void PostgameInterface::reinitialize(float gameTime)
{
}

void PostgameInterface::render()
{
    renderImage(IMAGE_BACKGROUND_POST_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderScore();

}

void PostgameInterface::renderScore()
{
    PostgameMenu* m = (PostgameMenu*)PostgameMenu::getInstance();
    string option = m->getCurrentPrompt();

    vector<EndGameStat> endGameStats = GAME_STATS->getEndGameStats();
    for (EndGameStat s : endGameStats)
    {

    }
}
