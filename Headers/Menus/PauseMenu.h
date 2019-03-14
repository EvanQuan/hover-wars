#pragma once

#include "Menus/PromptMenu.h"

class PauseMenu final : PromptMenu
{
public:
    ~PauseMenu();

    static Menu* getInstance();

private:
    PauseMenu();

    void select(eFixedCommand command);
    void back();

    static PauseMenu* m_pInstance;
};
