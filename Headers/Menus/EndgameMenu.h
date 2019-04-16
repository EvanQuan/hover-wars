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

    /*
        Time until the menu automatically moves onto the PostgameMenu
    */
    float m_fDelayTime;
};
