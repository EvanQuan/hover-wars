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

    // 
    virtual void update();

    /*
        Prompts are laid out in a 2D grid. The user can move between prompts
        within the grid, where the direction of their movement controls move
        the cursor. If the edge of the grid is reached, the cursor loops to the
        opposite end of the same row/column.

        Every prompt menu needs to instantiate a list of prompts.
    */
    vector<vector<const char*>> m_vPrompts;

    int m_iCurrentPromptX;
    int m_iCurrentPromptY;

private:
    PromptMenu();

protected:
    // void setPrompts(vector<vector<const char*>> prompts) { m_vPrompts = prompts; }
};
