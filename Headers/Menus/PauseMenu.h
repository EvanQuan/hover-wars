#pragma once

#include "Menus/PromptMenu.h"

class PauseMenu final : PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PauseMenu() {}

    static Menu* getInstance();

private:
    PauseMenu();

    void select(eFixedCommand command);
    // TODO should go back to in game
    void back() {}

    static PauseMenu* m_pInstance;
};
