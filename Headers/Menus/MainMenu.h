#pragma once

#include "Menus/PromptMenu.h"

class MainMenu final : PromptMenu
{
public:
    // TODO nothing to destruct?
    ~MainMenu() {}

    static Menu* getInstance();

private:
    MainMenu();

    void select(eFixedCommand command);

    void back();

    static MainMenu* m_pInstance;
};
