#include "Menus/PromptMenu.h"

/*
    If the user holds down a certain movement direction, there is a buffer time
    until the prompt movement repeats.

    Unit : seconds
*/
#define PROMPT_START_REPEAT_DELAY 0.5f
/*
    As the prompt repeats, there is a delay betweeen each repeat to ensure the
    cursor does not move too quickly.

    Unit : seconds
*/
#define PROMPT_REPEAT_DELAY 0.25f

/*
    To prevent very slight joystick movement from registering as cursor movement,
    the joystick direction vector must reach a certain magnitude before it
    registers.
    The possible magnitudes are between 0.0f - 1.0f
*/
#define PROMPT_JOYSTICK_MIN_MAGNITUDE 0.8f

PromptMenu::PromptMenu(vector<vector<pair<const char*, eFixedCommand>>> vPrompts) : Menu(
    // pressedKey
    unordered_map<int, eFixedCommand>
    {
        // Movement
        {GLFW_KEY_W,     COMMAND_PROMPT_UP},
        {GLFW_KEY_UP,    COMMAND_PROMPT_UP},
        {GLFW_KEY_A,     COMMAND_PROMPT_LEFT},
        {GLFW_KEY_LEFT,  COMMAND_PROMPT_LEFT},
        {GLFW_KEY_S,     COMMAND_PROMPT_DOWN},
        {GLFW_KEY_DOWN,  COMMAND_PROMPT_DOWN},
        {GLFW_KEY_D,     COMMAND_PROMPT_RIGHT},
        {GLFW_KEY_RIGHT, COMMAND_PROMPT_RIGHT},
    },
    // justPressedKey
    unordered_map<int, eFixedCommand>
    {
        // Select
        {GLFW_KEY_SPACE,     COMMAND_PROMPT_SELECT},
        {GLFW_KEY_ENTER,     COMMAND_PROMPT_SELECT},
        {GLFW_KEY_BACKSPACE, COMMAND_PROMPT_BACK},
        {GLFW_KEY_ESCAPE,    COMMAND_PROMPT_BACK},
    },
    // justReleasedKey
    unordered_map<int, eFixedCommand>
    {
        {GLFW_KEY_W,     COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_UP,    COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_A,     COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_LEFT,  COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_S,     COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_DOWN,  COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_D,     COMMAND_PROMPT_CURSOR_RELEASE},
        {GLFW_KEY_RIGHT, COMMAND_PROMPT_CURSOR_RELEASE},
    },
    // repeatButton
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_UP,     COMMAND_PROMPT_UP},
        {BUTTON_LEFT,   COMMAND_PROMPT_LEFT},
        {BUTTON_DOWN,   COMMAND_PROMPT_DOWN},
        {BUTTON_RIGHT,  COMMAND_PROMPT_RIGHT},
    },
    // justPressedButton
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_A,      COMMAND_PROMPT_SELECT},
        {TRIGGER_RIGHT, COMMAND_PROMPT_SELECT},
        {BUTTON_START,  COMMAND_PROMPT_SELECT},
        {BUTTON_B,      COMMAND_PROMPT_BACK},
        {TRIGGER_LEFT,  COMMAND_PROMPT_BACK},
        {BUTTON_BACK,   COMMAND_PROMPT_BACK},
    },
    // justReleasedButton
    unordered_map<int, eFixedCommand>
    {
        {BUTTON_UP,     COMMAND_PROMPT_CURSOR_RELEASE},
        {BUTTON_LEFT,   COMMAND_PROMPT_CURSOR_RELEASE},
        {BUTTON_DOWN,   COMMAND_PROMPT_CURSOR_RELEASE},
        {BUTTON_RIGHT,  COMMAND_PROMPT_CURSOR_RELEASE},
    }
)
{
    m_vPrompts = vPrompts;
    m_eCursorDirection = COMMAND_INVALID_FIXED;
    m_iCursorRow = 0;
    m_iCursorColumn = 0;

    m_fSecondsToNextRepeat = 0;
    m_fSecondsToStartRepeat = PROMPT_START_REPEAT_DELAY;
}


/*
    Before we check for keyboard input, set cursor direction to invalid
    (neutral).
*/
void PromptMenu::setupKeyCommands()
{
    m_eCursorDirectionKeyboard = COMMAND_INVALID_FIXED;
}

/*
    Move the keyboard cursor based on the current direction.

    NOTE: If there's a better way to do this, I'd like to know.

    @param direction    to move the keyboard cursor
*/
void PromptMenu::moveCursorKeyboard(eFixedCommand direction)
{
    switch (m_eCursorDirectionKeyboard)
    {
    case COMMAND_INVALID_FIXED: // Neutral
        switch (direction)
        {
        case COMMAND_PROMPT_UP:
        case COMMAND_PROMPT_RIGHT:
        case COMMAND_PROMPT_DOWN:
        case COMMAND_PROMPT_LEFT:
            m_eCursorDirectionKeyboard = direction;
            break;
        }
        break;
    case COMMAND_PROMPT_UP:
        switch (direction)
        {
        case COMMAND_PROMPT_RIGHT:
        case COMMAND_PROMPT_LEFT:
            m_eCursorDirectionKeyboard = direction;
            break;
        case COMMAND_PROMPT_DOWN:
            m_eCursorDirectionKeyboard = COMMAND_INVALID_FIXED;
            break;
        }
    case COMMAND_PROMPT_DOWN:
        switch (direction)
        {
        case COMMAND_PROMPT_RIGHT:
        case COMMAND_PROMPT_LEFT:
            m_eCursorDirectionKeyboard = direction;
            break;
        case COMMAND_PROMPT_UP:
            m_eCursorDirectionKeyboard = COMMAND_INVALID_FIXED;
            break;
        }
    case COMMAND_PROMPT_RIGHT:
        switch (direction)
        {
        case COMMAND_PROMPT_UP:
        case COMMAND_PROMPT_DOWN:
            m_eCursorDirectionKeyboard = direction;
            break;
        case COMMAND_PROMPT_LEFT:
            m_eCursorDirectionKeyboard = COMMAND_INVALID_FIXED;
            break;
        }
    case COMMAND_PROMPT_LEFT:
        switch (direction)
        {
        case COMMAND_PROMPT_UP:
        case COMMAND_PROMPT_DOWN:
            m_eCursorDirectionKeyboard = direction;
            break;
        case COMMAND_PROMPT_RIGHT:
            m_eCursorDirectionKeyboard = COMMAND_INVALID_FIXED;
            break;
        }
    }
}

// @EvanQuan : hovercraft not referenced
// Not referenced due to inherited virtual function
void PromptMenu::executeFixedCommand(eHovercraft hovercraft, eFixedCommand command)
{
    switch (command)
    {
    case COMMAND_PROMPT_UP:
    case COMMAND_PROMPT_LEFT:
    case COMMAND_PROMPT_DOWN:
    case COMMAND_PROMPT_RIGHT:
        if (m_fSecondsToNextRepeat <= 0)
        {
            moveCursor(command);
        }
        break;
    case COMMAND_PROMPT_CURSOR_RELEASE:
        releaseCursor();
        break;
    case COMMAND_PROMPT_SELECT:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_SELECT);
        select(getCurrentPromptCommand());
        break;
    case COMMAND_PROMPT_BACK:
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_SELECT);
        back();
        break;
    case COMMAND_CLOSE_WINDOW:
        cout << "PromptMenu CLOSE WINDOW" << endl;
        GAME_MANAGER->flagWindowToClose();
        break;
    }
}

int PromptMenu::getMaxRowCount()
{
    return m_vPrompts.size();
}


int PromptMenu::getMaxRowIndex()
{
    return getMaxRowCount() - 1;
}

int PromptMenu::getMaxColumnCount()
{
    return m_vPrompts.at(m_iCursorRow).size();
}

int PromptMenu::getMaxColumnIndex()
{
    return getMaxColumnCount() - 1;
}

/*
    Move the cursor down 1 row.

    @return true if actually moved down a row
*/
bool PromptMenu::moveDownRow()
{
    int oldCursorRow = m_iCursorRow;
    int maxRowCount = getMaxRowCount();
    // Loop down row
    m_iCursorRow = (m_iCursorRow + 1) % maxRowCount;
    // Reset column if necessary
    int newMaxColumnIndex = getMaxColumnIndex();
    if (m_iCursorColumn > newMaxColumnIndex)
    {
        m_iCursorColumn = newMaxColumnIndex;
    }

    return oldCursorRow != m_iCursorRow;
}

/*
    Move the cursor up 1 row.

    @return true if actually moved up a row
*/
bool PromptMenu::moveUpRow()
{
    int oldCursorRow = m_iCursorRow;
    int maxRowCount = getMaxRowCount();
    // Loop up row, prevent negative modulo
    m_iCursorRow = (((m_iCursorRow - 1) % maxRowCount) + maxRowCount) % maxRowCount;
    // Reset column if necessary
    int newMaxColumnIndex = getMaxColumnIndex();
    if (m_iCursorColumn > newMaxColumnIndex)
    {
        m_iCursorColumn = newMaxColumnIndex;
    }

    return oldCursorRow != m_iCursorRow;

}

/*
    Move the cursor right 1 column

    @return true if actually moved right a column
*/
bool PromptMenu::moveRightColumn()
{
    int oldCursorColumn = m_iCursorColumn;
    int maxColumnCount = getMaxColumnCount();
    // Loop right column
    m_iCursorColumn = (m_iCursorColumn + 1) % maxColumnCount;

    return oldCursorColumn != m_iCursorColumn;
}

/*
    Move the cursor left 1 column

    @return true if actually moved left a column
*/
bool PromptMenu::moveLeftColumn()
{
    int oldCursorColumn = m_iCursorColumn;
    int maxColumnCount = getMaxColumnCount();
    // Loop left column, prevent negative modulo
    m_iCursorColumn = (((m_iCursorColumn - 1) % maxColumnCount) + maxColumnCount) % maxColumnCount;

    return oldCursorColumn != m_iCursorColumn;
}

/*
    Move the cursor in the menu in the menu prompt map, based on the direction.
    If the cursor goes beyond the bounds of the prompt map, it will re-enter on
    the other side (modulo) so that the cursor cycles its movement.
*/
void PromptMenu::moveCursor(eFixedCommand direction)
{
    //if (m_fSecondsToNextRepeat > 0.0f) {
        //return;
    //}
    m_fSecondsToNextRepeat = PROMPT_REPEAT_DELAY;
    bool moved = false;
    switch (direction)
    {
    case COMMAND_PROMPT_UP:
        moved = moveUpRow();
        break;
    case COMMAND_PROMPT_LEFT:
        moved = moveLeftColumn();
        break;
    case COMMAND_PROMPT_DOWN:
        moved = moveDownRow();
        break;
    case COMMAND_PROMPT_RIGHT:
        moved = moveRightColumn();
        break;
    default:
        return; // end early as it was not a cursor movement command
    }
    if (moved)
    {
        // We have modulo checks to keep the cursor looping within valid options
        // However, if it loops through a dimension with only 1 option, the cursor
        // hasn't actually changed in practice. As a result, we do not want to play
        // a cursor move sound to signify the cursor has not moved.
        SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_CURSOR_MOVE);
        // debug
        // cout << "Cursor moved to (" << m_iCursorRow << ", " << m_iCursorColumn << ") " << endl;
        cout << "> " << getCurrentPrompt() << endl;
    }
}

/*
    The the user has just released cursor movement, which resets the start
    repeat delay.
*/
void PromptMenu::releaseCursor()
{
    m_fSecondsToStartRepeat = PROMPT_START_REPEAT_DELAY;
    // m_fSecondsToNextRepeat = PROMPT_REPEAT_DELAY;
    m_eCursorDirection = COMMAND_INVALID_FIXED;
}

void PromptMenu::enter()
{
    m_iCursorRow = 0;
    m_iCursorColumn = 0;
    cout << "\n" << "Changed to " << this << " menu:" << "\n> " << getCurrentPrompt() << endl;
}


void PromptMenu::updateTimeValues(float fTimeInSeconds)
{
    m_fSecondsToStartRepeat -= fTimeInSeconds;
    if ((m_fSecondsToStartRepeat <= 0.0f)
        && (m_fSecondsToNextRepeat > 0.0f))
    {
        m_fSecondsToNextRepeat -= fTimeInSeconds;
    }
}

//@EvanQuan : Functions not implemented
void PromptMenu::handleAccumulatedKeyCommands(eFixedCommand command)
{
    moveCursorKeyboard(command);
}

void PromptMenu::executeAccumulatedKeyCommands(eHovercraft hovercraft)
{
    executeFixedCommand(hovercraft, m_eCursorDirectionKeyboard);
}

void PromptMenu::updateLeftStick(eHovercraft hovercraft, float x, float y)
{
    executeFixedCommand(hovercraft, joystickStateToPromptDirection(x, y));
}

void PromptMenu::updateRightStick(eHovercraft hovercraft, float x, float y)
{
    executeFixedCommand(hovercraft, joystickStateToPromptDirection(x, y));
}

/*
Joysticks can be used to nagivate the menu. Since joystick values are not
discrete in their direction like keys or buttons are, we need to be able to
find out what direction the joysticks are headed towards.

@param x    x-coordinate of joystick
@param y    y-coordinate of joystick
@return the prompt direction the joystick is moving in.
        If th magnitude of the joystick is at least
        PROMPT_JOYSTICK_MIN_MAGNITUDE, the direction can be:
            COMMAND_PROMPT_RIGHT
            COMMAND_PROMPT_LEFT
            COMMAND_PROMPT_UP
            COMMAND_PROMPT_DOWN
        Otherwise, the direction will be:
            COMMAND_INVALID_FIXED
        and will not register as a direction.

*/
eFixedCommand PromptMenu::joystickStateToPromptDirection(float x, float y)
{
    float magnitude = FuncUtils::getMagnitude(x, y);
    if (magnitude < PROMPT_JOYSTICK_MIN_MAGNITUDE)
    {
        if (magnitude <= 0)
        {
            releaseCursor();
        }
        return COMMAND_INVALID_FIXED;
    }
    if (abs(x) > abs(y)) // x takes priority
    {
        return x > 0 ? COMMAND_PROMPT_RIGHT : COMMAND_PROMPT_LEFT;
    }
    else // y takes priority
    {
        return y > 0 ? COMMAND_PROMPT_UP : COMMAND_PROMPT_DOWN;
    }

}
