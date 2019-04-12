#pragma once

#include "Menus/PromptMenu.h"
#include "GameStats.h"

class AwardsMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~AwardsMenu() {}

    static Menu* getInstance();

private:
    AwardsMenu();

    void select(eFixedCommand command);

    // Override
    void back();

    // @Override
    void enter();

    static AwardsMenu* m_pInstance;
};
