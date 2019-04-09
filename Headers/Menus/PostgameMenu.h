#pragma once

#include "Menus/PromptMenu.h"
#include "GameStats.h"

class PostgameMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PostgameMenu() {}

    static Menu* getInstance();

    int getBotCount() const { return m_iBotCount; }
    int getPlayerCount() const { return m_iPlayerCount; }
    vector<EndGameStat> getEndGameStats() const { return endGameStats; }
private:
    PostgameMenu();

    void enter();

    void select(eFixedCommand command);

    void back();

    // @Override
    void updateTimeValues(float fTimeInSeconds);

    vector<EndGameStat> endGameStats;

    int m_iBotCount;
    int m_iPlayerCount;

    static PostgameMenu* m_pInstance;

    bool m_bIgnoreUserInput;
    float m_fIgnoreUserInputTime;
};
