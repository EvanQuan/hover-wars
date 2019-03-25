#pragma once

#include "Menus/PromptMenu.h"

class PregameMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PregameMenu() {}

    static Menu* getInstance();
    int getPlayerCount() { return m_iPlayerCount; };
    int getBotCount() { return m_iBotCount; };
    int getGameTime() { return m_fGameTime; };

private:
    PregameMenu();

    void select(eFixedCommand command);

    void moveCursor(eFixedCommand command);

    void back();

    int m_iBotCount;
    int m_iPlayerCount;
    int m_fGameTime;

    static PregameMenu* m_pInstance;
};
