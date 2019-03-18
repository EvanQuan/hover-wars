#pragma once

#include "Menus/PromptMenu.h"

class PregameMenu final : PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PregameMenu() {}

    static Menu* getInstance();

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
