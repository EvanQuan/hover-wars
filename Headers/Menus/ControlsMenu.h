#pragma once

#include "Menus/PromptMenu.h"

class ControlsMenu final : public PromptMenu
{
public:

    ~ControlsMenu() {}

    static Menu* getInstance();

private:
    ControlsMenu();

    void select(eFixedCommand command);

    // ControlsMenu cannot go back to anything
    void back();

    // @Override
    void enter();

    static ControlsMenu* m_pInstance;
};
