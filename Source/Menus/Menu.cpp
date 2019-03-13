#include "Menus/Menu.h"
#include "CommandHandler.h"

Menu::Menu()
{

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
