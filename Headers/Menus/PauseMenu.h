#pragma once

#include "Menus/PromptMenu.h"

class PauseMenu final : PromptMenu
{
public:
    ~PauseMenu();

    // static Menu* getInstance();
    // virtual void update();

private:
    PauseMenu();

    static PauseMenu* m_pInstance;
};
