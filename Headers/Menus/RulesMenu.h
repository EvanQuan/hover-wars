#pragma once

#include "Menus/PromptMenu.h"

class RulesMenu final : public PromptMenu
{
public:

    ~RulesMenu() {}

    static Menu* getInstance();

private:
    RulesMenu();

    void select(eFixedCommand command);

    // RulesMenu cannot go back to anything
    void back();

    // @Override
    void enter();

    static RulesMenu* m_pInstance;
};
