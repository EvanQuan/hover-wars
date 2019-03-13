#pragma once

#include "Menus/Menu.h"

/*
    Prompt menus  represent out of game menus. These menus are distinct in that
    they have always have a list of possible prompts. The user can have a
    cursor over any given prompt, and select any. By selecting a prompt, there
    will be some state change.

    @author Evan Quan
*/
class PromptMenu abstract : Menu
{
public:
    ~PromptMenu();

    /*
        Prompts are laid out in a 2D grid. The user can move between prompts
        within the grid, where the direction of their movement controls move
        the cursor. If the edge of the grid is reached, the cursor loops to the
        opposite end of the same row/column.

        Every prompt menu needs to instantiate a list of prompts. This should
        be done in the constructor.

        Public to be accessible to the UserInterface
    */
    vector<vector<const char*>> m_vPrompts;

    // x-coordinate of current prompt in m_vPrompts
    int m_iCurrentPromptX;
    // y-coordinate of current prompt in m_vPrompts
    int m_iCurrentPromptY;

private:
    float m_fSecondsToStartRepeat;
    float m_fSecondsToNextRepeat;

    eFixedCommand joystickStateToPromptDirection(float x, float y);

protected:
    PromptMenu();
    // void setPrompts(vector<vector<const char*>> prompts) { m_vPrompts = prompts; }

    virtual void select() = 0;
    virtual void back() = 0;

    void executeFixedCommand(eHovercraft hovercraft, eFixedCommand command);

    // For keyboard command handling
    void setupKeyCommands();
    void handleAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command);
    void executeAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command);

    // Joystick commands
    void updateLeftStick(eHovercraft hovercraft, float x, float y);
    void updateRightStick(eHovercraft hovercraft, float x, float y);

};
