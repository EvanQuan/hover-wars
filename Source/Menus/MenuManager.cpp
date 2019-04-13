#include "Menus/MenuManager.h"
#include "Menus/StartMenu.h"
#include "Menus/GameMenu.h"
#include "UserInterface/GameInterface.h"

// Singleton instance
MenuManager* MenuManager::m_pInstance = nullptr;
/*
Constructor
*/
MenuManager::MenuManager()
{
}

/*
Get Singleton instance
*/
MenuManager* MenuManager::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new MenuManager();
        // Set current menu is done after after the constructor to prevent
        // mutual recursion
    }

    return m_pInstance;
}

/*
Destructor
*/
MenuManager::~MenuManager()
{
    // Delete all menu instances, which are created as needed as the player
    // navigates between menus
    for (Menu* menu : m_vMenuInstances)
    {
        delete menu;
    }
    m_vMenuInstances.clear();
}

void MenuManager::setCurrentMenu(Menu* menu)
{
    m_pCurrentMenu = menu;
    menu->enter();
}


/*
Execute all the commands for a given frame. This should be called every frame
update.
*/
void MenuManager::update(float fTimeInSeconds)
{
    m_pCurrentMenu->update(fTimeInSeconds);
}

void MenuManager::addMenu(Menu* menu)
{
    m_vMenuInstances.push_back(menu);
}
