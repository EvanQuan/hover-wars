#pragma once

#include "Menus/Menu.h"

/*
    Prompt menus  represent out of game menus. These menus are distinct in that
    they have always have a list of possible prompts. The user can have a
    cursor over any given prompt, and select any. By selecting a prompt, there
    will be some state change.

    @author Evan Quan
*/
class PromptMenu abstract : public Menu
{
public:
    // TODO nothing to desctruct?
    ~PromptMenu() {}

    /*
        Prompts are laid out in a 2D grid. The user can move between prompts
        within the grid, where the direction of their movement controls move
        the cursor. If the edge of the grid is reached, the cursor loops to the
        opposite end of the same row/column.

        Every prompt menu needs to instantiate a list of prompts. This should
        be done in the constructor.

        Public to be accessible to the UserInterface. Should never be modified
        after instantiation.
    */
    vector<vector<pair<const char*, eFixedCommand>>> m_vPrompts;

    // x-coordinate of current prompt in m_vPrompts
    int m_iCursorRow;
    // y-coordinate of current prompt in m_vPrompts
    int m_iCursorColumn;

    const char* getCurrentPrompt() const { return m_vPrompts.at(m_iCursorRow).at(m_iCursorColumn).first; }

private:
    float m_fSecondsToStartRepeat;
    float m_fSecondsToNextRepeat;

    eFixedCommand joystickStateToPromptDirection(float x, float y);

    void updateTimeValues(float fTimeInSeconds);

    int getMaxRowIndex();
    int getMaxColumnIndex();

    int getMaxRowCount();
    int getMaxColumnCount();

    bool moveDownRow();
    bool moveUpRow();

    bool moveRightColumn();
    bool moveLeftColumn();

    void releaseCursor();
    eFixedCommand m_eCursorDirection;


protected:
    PromptMenu(vector<vector<pair<const char*, eFixedCommand>>> vPrompts);
    // void setPrompts(vector<vector<const char*>> prompts) { m_vPrompts = prompts; }

    // If overriden, call the parent version
    virtual void moveCursor(eFixedCommand direction);
    // Process the current selected command.
    // This is called as the user selects the prompt under the current cursor
    // location.
    virtual void select(eFixedCommand command) = 0;
    // The back button(s) have their own behaviour. Ideally this behaviour is
    // independent of the cursor position, and should move the user to some
    // other menu.
    virtual void back() = 0;

    void enter();

    eFixedCommand getCurrentPromptCommand() const { return m_vPrompts.at(m_iCursorRow).at(m_iCursorColumn).second; }

    void executeFixedCommand(eHovercraft hovercraft, eFixedCommand command);

    // For keyboard command handling
    void setupKeyCommands();
    void handleAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command);
    void executeAccumulatedKeyCommands(eHovercraft hovercraft, eFixedCommand command);

    // Joystick commands
    void updateLeftStick(eHovercraft hovercraft, float x, float y);
    void updateRightStick(eHovercraft hovercraft, float x, float y);

};
