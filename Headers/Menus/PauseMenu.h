#pragma once

#include "Menus/PromptMenu.h"

class PauseMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PauseMenu() {}

    static Menu* getInstance();

private:
    PauseMenu();

    void select(eFixedCommand command);

    void back();

    void enter();

    static PauseMenu* m_pInstance;
};
