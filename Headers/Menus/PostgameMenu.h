#pragma once

#include "Menus/PromptMenu.h"

class PostgameMenu final : PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PostgameMenu() {}

    static Menu* getInstance();

private:
    PostgameMenu();

    void select(eFixedCommand command);

    void back();

    int m_iBotCount;
    int m_iPlayerCount;

    static PostgameMenu* m_pInstance;
};
