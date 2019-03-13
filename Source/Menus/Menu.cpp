#include "Menus/Menu.h"
#include "CommandHandler.h"

Menu::Menu(
    unordered_map<eVariableCommand, const char*> eVariableCommandToString,
    unordered_map<int, eFixedCommand> m_pressedKeyToFixedCommand,
    unordered_map<int, eFixedCommand> m_justPressedKeyToFixedCommand,
    unordered_map<int, eFixedCommand> m_justReleasedKeyToFixedCommand,
    unordered_map<int, eFixedCommand> m_repeatButtonToFixedCommand,
    unordered_map<int, eFixedCommand> m_justPressedButtonToFixedCommand,
    unordered_map<int, eFixedCommand> m_justReleasedButtonToFixedCommand
)
{
    this->eVariableCommandToString = eVariableCommandToString;
    this->m_pressedKeyToFixedCommand = m_pressedKeyToFixedCommand;
    this->m_justPressedKeyToFixedCommand = m_justPressedKeyToFixedCommand;
    this->m_justReleasedKeyToFixedCommand = m_justReleasedKeyToFixedCommand;
    this->m_repeatButtonToFixedCommand = m_repeatButtonToFixedCommand;
    this->m_justPressedButtonToFixedCommand = m_justPressedButtonToFixedCommand;
    this->m_justReleasedButtonToFixedCommand = m_justReleasedButtonToFixedCommand;

    m_pInputHandler = InputHandler::getInstance();
    bWireFrameEnabled = false;
}

Menu::~Menu()
{
    // Nothing to delete
}

/*
Change the CommandHandler's next menu
*/
void Menu::nextMenu(Menu* next)
{
    COMMAND_HANDLER->m_pCurrentMenu = next;
}

// Toggles Wireframe drawing
void Menu::debugToggleWireframe()
{
    // Toggle Boolean
    bWireFrameEnabled = !bWireFrameEnabled;

    // Set Polygon mode based on current setting.
    if (bWireFrameEnabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
