#pragma once

#include "Menus/PromptMenu.h"

class StartMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~StartMenu() {}

    static Menu* getInstance();

private:
    StartMenu();

    void select(eFixedCommand command);

    // StartMenu cannot go back to anything
    void back() {}

    static StartMenu* m_pInstance;
};
