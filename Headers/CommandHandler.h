#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "SoundManager.h"
// #include "Menus/Menu.h"
class Menu;

#ifndef NDEBUG
    #include "UserInterface/UserInterface.h"
#endif

/* CLASS */
/*
Receives user command initiates appropriate actions that correspond to that
command. Usually these commands would come from InputHandler (user input), but
may come from other means for test purposes.

@author Evan Quan
*/
class CommandHandler final
{
public:
    static CommandHandler* getInstance(GLFWwindow *rWindow);
    static CommandHandler* getInstance();
    ~CommandHandler();

    // Execute all commands for a given frame.
    void update();

    void addMenu(Menu* menu);
    void setCurrentMenu(Menu* menu);

    GLFWwindow* m_pWindow;
    Menu* m_pCurrentMenu;
private:
    // Singleton Variables
    CommandHandler(GLFWwindow *rWindow);
    static CommandHandler* m_pInstance;

    // For memory management
    vector<Menu*> m_vMenuInstances;
};
