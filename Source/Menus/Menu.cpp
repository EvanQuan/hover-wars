#include "Menus/Menu.h"
#include "CommandHandler.h"

Menu::Menu()
{
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
