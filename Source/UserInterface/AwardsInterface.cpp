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
    renderImage(IMAGE_BACKGROUND_POST_MENU, m_vComponentCoordinates[BACKGROUND].first, m_vComponentCoordinates[BACKGROUND].second, 1.0f);
    renderAwards();
}

void AwardsInterface::renderOption() {

}

void AwardsInterface::renderAwards() {
    vector<EndGameStat> endGameStats = GAME_STATS->getEndGameStats();


    string s_playername;
    string s_awardname;

     

    GLfloat x = 100;
    GLfloat y = 1000;
    for (int i = 0, size = endGameStats.size(); i < size; i++)
    {
        eHovercraft hovercraft = endGameStats.at(i).hovercraft;
        vector<Award, allocator<Award>> awards = endGameStats.at(i).awards;

        switch(hovercraft) {
            case HOVERCRAFT_PLAYER_1:
                s_playername = "Player 1";
                break;
            case HOVERCRAFT_PLAYER_2:
                s_playername = "Player 2";
                break;
            case HOVERCRAFT_PLAYER_3:
                s_playername = "Player 3";
                break;
            case HOVERCRAFT_PLAYER_4:
                s_playername = "Player 4";
                break;
            case HOVERCRAFT_BOT_1:
                s_playername = "Bot 1";
                break;
            case HOVERCRAFT_BOT_2:
                s_playername = "Bot 2";
                break;
            case HOVERCRAFT_BOT_3:
                s_playername = "Bot 3";
                break;
            case HOVERCRAFT_BOT_4:
                s_playername = "Bot 4";
                break;
        }

        renderText(s_playername, x, y, 1.0f, vec3(1.0f));
        y -= 50;

        //int awards_size = sizeof(awards) / sizeof(awards[0]);
        //cout << "awards_size: " << awards.size << endl;

        for (int i = 0; i < (int) awards.size(); i++) {
            string name = awards[i].name;
            string description = awards[i].description;
            int points = awards[i].points;
            int statValue = awards[i].statValue;

            char s_points[3];
            char s_statValue[4];

            sprintf(s_points, "%d", points);
            sprintf(s_statValue, "%d", statValue);

            string sentence = name + ": \"" + description + " of " + s_points + "\" +" + s_statValue;
            renderText(sentence, x, y, 1.0f, vec3(1.0f));
            y -= 50;
        }
        y -= 50;
    }
    //renderText("Award 0 with Survivor: \"Least deaths of 0\" +500", 500, 500, 1.0f, vec3(1.0f));
}

int AwardsInterface::compare(const void*a, const void*b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);

    if (int_a == int_b) return 0;
    else if (int_a < int_b) return -1;
    else return 1;
}
