#pragma once

#include "Menus/PromptMenu.h"

class PauseMenu final : public PromptMenu
{
public:
    // TODO nothing to destruct?
    ~PauseMenu() {}

    static Menu* getInstance();

    void setPauser(eHovercraft pauser) { m_ePauser = pauser; }
    /*
        @return the player that paused the game. Only this player can control
        the pause menu.
    */
    eHovercraft getPauser() const { return m_ePauser; }
private:
    PauseMenu();

    void select(eFixedCommand command);

    void back();

    void enter();

    // @Override
    void executeFixedCommand(eHovercraft hovercraft, eFixedCommand command);

    /*
        The player that paused the game. Only this player is allowed to
        navigate the pause menu to prevent accidental (or intentional) unpause
        from other players.
    */
    eHovercraft m_ePauser;
    static PauseMenu* m_pInstance;
};
