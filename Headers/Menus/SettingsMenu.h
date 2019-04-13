#pragma once

#include "Menus/PromptMenu.h"

class SettingsMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~SettingsMenu() {}

    static SettingsMenu* getInstance();

    /*
        Check if background music should play for the menus, in-game, and for
        the pause menu.

        @return true if the setting to have music enabled  is on.
    */
    bool hasMusicEnabled() const { return m_bMusicEnabled; }

private:
    SettingsMenu();

    void select(eFixedCommand command);

    // @Override
    bool moveCursorOverride(eFixedCommand command);

    void back();

    // @Override
    void enter();

    static SettingsMenu* m_pInstance;

    bool m_bMusicEnabled;
};
