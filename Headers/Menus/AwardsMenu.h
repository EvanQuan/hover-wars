#pragma once

#include "Menus/PromptMenu.h"

class AwardsMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~AwardsMenu() {}

    static Menu* getInstance();

private:
    AwardsMenu();

    void select(eFixedCommand command);

    // AwardsMenu cannot go back to anything
    void back() {}

    // @Override
    void enter();

    static AwardsMenu* m_pInstance;
};
