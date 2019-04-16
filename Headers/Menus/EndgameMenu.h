#pragma once

#include "Menus/PromptMenu.h"

class EndgameMenu final : public PromptMenu
{
public:

    ~EndgameMenu() {}

    static Menu* getInstance();

private:
    EndgameMenu();

    void select(eFixedCommand command);

    // EndgameMenu cannot go back to anything
    void back();

    // @Override
    void enterOverride();

    // @Override
    void updateTimeValueOverride(float fTimeInSeconds);

    static EndgameMenu* m_pInstance;

    float m_fDelayTime;
};
