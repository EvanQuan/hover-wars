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

    void back();

    static PregameMenu* m_pInstance;
};
