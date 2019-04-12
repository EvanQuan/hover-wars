#pragma once
#include "UserInterface/MenuInterface.h"

#define BACK_TO_MAIN_MENU_OPTION "Return to Main Menu"
#define AWARDS_OPTION "Awards"

struct EndGameStat;

class PostgameInterface final : public MenuInterface
{
public:

    static PostgameInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PostgameInterface();
    PostgameInterface(const PostgameInterface* pCopy);                              // Default Copy Constructor
    PostgameInterface& operator=(const PostgameInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:

    enum eUIComponent
    {
        RANK_1 = 0,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        BACKGROUND,
        PLACEMENT,
        AWARDS,
        NUMBER_1,
        NUMBER_2,
        NUMBER_3,
        RANK_1_SCORE_THOUSAND,
        RANK_1_SCORE_HUNDRED,
        RANK_1_SCORE_TEN,
        RANK_1_SCORE_ONE,
        RANK_1_POINT_ICON,

        RANK_2_SCORE_THOUSAND,
        RANK_2_SCORE_HUNDRED,
        RANK_2_SCORE_TEN,
        RANK_2_SCORE_ONE,
        RANK_2_POINT_ICON,

        RANK_3_SCORE_THOUSAND,
        RANK_3_SCORE_HUNDRED,
        RANK_3_SCORE_TEN,
        RANK_3_SCORE_ONE,
        RANK_3_POINT_ICON,

        RANK_4_SCORE_THOUSAND,
        RANK_4_SCORE_HUNDRED,
        RANK_4_SCORE_TEN,
        RANK_4_SCORE_ONE,
        RANK_4_POINT_ICON,

        RANK_5_SCORE_THOUSAND,
        RANK_5_SCORE_HUNDRED,
        RANK_5_SCORE_TEN,
        RANK_5_SCORE_ONE,
        RANK_5_POINT_ICON,

        RANK_6_SCORE_THOUSAND,
        RANK_6_SCORE_HUNDRED,
        RANK_6_SCORE_TEN,
        RANK_6_SCORE_ONE,
        RANK_6_POINT_ICON,

        RANK_7_SCORE_THOUSAND,
        RANK_7_SCORE_HUNDRED,
        RANK_7_SCORE_TEN,
        RANK_7_SCORE_ONE,
        RANK_7_POINT_ICON,

        RANK_8_SCORE_THOUSAND,
        RANK_8_SCORE_HUNDRED,
        RANK_8_SCORE_TEN,
        RANK_8_SCORE_ONE,
        RANK_8_POINT_ICON,

        MAIN_MENU,
    };
    static PostgameInterface* m_pInstance;

    void renderPlacement();
    void renderFreeForAll(const vector<EndGameStat> &endGameStats);
    void renderTeamBots(const vector<EndGameStat> &endGameStats);
    void renderPlayersVsBots(const vector<EndGameStat> &endGameStats);
    void renderTeamPlayers(const vector<EndGameStat> &endGameStats);
    void renderScore(int place, int score);
    void renderOption();
};
