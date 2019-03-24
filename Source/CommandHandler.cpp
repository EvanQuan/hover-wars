#include "CommandHandler.h"
#include "Menus/StartMenu.h"
#include "Menus/GameMenu.h"
#include "UserInterface/GameInterface.h"

// Singleton instance
CommandHandler* CommandHandler::m_pInstance = nullptr;
/*
Constructor
*/
CommandHandler::CommandHandler()
{
}

/*
Get Singleton instance
*/
CommandHandler* CommandHandler::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new CommandHandler();
        // Set current menu is done after after the constructor to prevent
        // mutual recursion
#ifdef NDEBUG
        // m_pInstance->setCurrentMenu(StartMenu::getInstance());
#else
        // Release
        // GAME_MANAGER->initializeNewGame(1, 4, 9999999.0f, RELEASE_ENV);
        // m_pInstance->setCurrentMenu(GameMenu::getInstance());
        // GAME_MANAGER->m_pUserInterface = GameInterface::getInstance(GAME_MANAGER->m_iWidth, GAME_MANAGER->m_iHeight);
        // DEBUG
#endif // !NDEBUG



    }

    return m_pInstance;
}

/*
Destructor
*/
CommandHandler::~CommandHandler()
{
    // Delete all menu instances, which are created as needed as the player
    // navigates between menus
    for (Menu* menu : m_vMenuInstances)
    {
        delete menu;
    }
    m_vMenuInstances.clear();
}

void CommandHandler::setCurrentMenu(Menu* menu)
{
    m_pCurrentMenu = menu;
    menu->enter();
}


/*
Execute all the commands for a given frame. This should be called every frame
update.
*/
void CommandHandler::update(float fTimeInSeconds)
{
    m_pCurrentMenu->update(fTimeInSeconds);
}

void CommandHandler::addMenu(Menu* menu)
{
    m_vMenuInstances.push_back(menu);
}
