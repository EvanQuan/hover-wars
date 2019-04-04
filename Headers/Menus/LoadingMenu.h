#pragma once

#include "Menus/PromptMenu.h"

class LoadingMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~LoadingMenu() {}

    static Menu* getInstance();

private:
    LoadingMenu();

    void select(eFixedCommand command);

    // LoadingMenu cannot go back to anything
    void back() {}

    static LoadingMenu* m_pInstance;
};
