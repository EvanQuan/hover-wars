#pragma once

#include "Menus/PromptMenu.h"
#include "GameStats.h"

class PostgameMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PostgameMenu() {}

    static Menu* getInstance();

private:
    PostgameMenu();

    void enter();

    void select(eFixedCommand command);

    void back();

    vector<EndGameStat> endGameStats;

    int m_iBotCount;
    int m_iPlayerCount;

    static PostgameMenu* m_pInstance;
};
