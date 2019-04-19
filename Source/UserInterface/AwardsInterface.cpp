#include "UserInterface/AwardsInterface.h"
#include "GameManager.h"
#include "Menus/AwardsMenu.h"

// Singleton instance
AwardsInterface* AwardsInterface::m_pInstance = nullptr;

AwardsInterface::AwardsInterface() : PromptInterface(
    // Scaling
    vector<pair<float, float>>
    {
        // 1 Title
        {0.2f, 0.95f},
    },
    // Translating
    vector<pair<float, float>>
    {
        // 1 Title
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
    renderBackgroundImage(IMAGE_BACKGROUND_POST_MENU);
    renderText("Awards", TITLE, COLOR_WHITE);
    renderAwards();
}

void AwardsInterface::renderOption() {

}

/*
    @TODO Make these dynamic on the window dimensions.
*/
void AwardsInterface::renderAwards() {
    vector<EndGameStat> endGameStats = GAME_STATS->getEndGameStats();


    string s_playername;
    string s_awardname;

    int i_count = 0;
    for each (EndGameStat stat in endGameStats) {
        if (!stat.awards.empty())
        {
            i_count++;
        }
    }

    GLfloat x = 100;
    GLfloat y = 950;
    int i_done = 0;
    int k = 0;
    while (k < (int)endGameStats.size())
    {
        eHovercraft hovercraft = endGameStats.at(k).hovercraft;
        if (GAME_STATS->isBot(hovercraft))
        {
            k++;
            continue;
        }

        vector<Award, allocator<Award>> awards = endGameStats.at(k).awards;
        if (!awards.empty()) {
            renderText(FuncUtils::toString(hovercraft), x, y, 1.0f, GAME_MANAGER->getHovercraftColor(hovercraft));
            y -= 50;

            for each (Award award in awards) {
                string name = award.name;
                string description = award.description;
                int points = award.points;
                int statValue = award.statValue;

                char s_points[3];
                char s_statValue[4];

                sprintf(s_points, "%d", points);
                sprintf(s_statValue, "%d", statValue);

                string sentence = name + ": \"" + description + " of " + s_statValue + "\" +" + s_points;
                renderText(sentence, x, y, 1.0f, COLOR_WHITE);
                y -= 50;
            }
            y -= 50;
            i_done++;
        }
        k++;
        if (i_done >= i_count / 2 + 1) break;
    }

    x = 1000;
    y = 950;
    for (k; k < (int)endGameStats.size(); k++)
    {
        eHovercraft hovercraft = endGameStats.at(k).hovercraft;
        if (GAME_STATS->isBot(hovercraft))
        {
            continue;
        }

        vector<Award, allocator<Award>> awards = endGameStats.at(k).awards;
        if (!awards.empty()) {
            renderText(FuncUtils::toString(hovercraft), x, y, 1.0f, GAME_MANAGER->getHovercraftColor(hovercraft));
            y -= 50;

            for each (Award award in awards) {

                string name = award.name;
                string description = award.description;
                int points = award.points;
                int statValue = award.statValue;

                char s_points[3];
                char s_statValue[4];

                sprintf(s_points, "%d", points);
                sprintf(s_statValue, "%d", statValue);

                string sentence = name + ": \"" + description + " of " + s_statValue + "\" +" + s_points;
                renderText(sentence, x, y, 1.0f, vec3(1.0f));
                y -= 50;
            }
            y -= 50;
        }
    }

}
