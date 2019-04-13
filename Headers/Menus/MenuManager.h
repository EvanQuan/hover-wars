#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "SoundManager.h"

class Menu;

#ifndef NDEBUG
    #include "UserInterface/UserInterface.h"
#endif

/* CLASS */
/*
    Stores and manages Menu instances.

    @author Evan Quan
*/
class MenuManager final
{
public:
    static MenuManager* getInstance();
    ~MenuManager();

    // Execute all commands for a given frame.
    void update(float fTimeInSeconds);

    void addMenu(Menu* menu);
    void setCurrentMenu(Menu* menu);

    GLFWwindow* m_pWindow;
private:
    // Singleton Variables
    MenuManager();
    static MenuManager* m_pInstance;

    Menu* m_pCurrentMenu;
    // For memory management
    vector<Menu*> m_vMenuInstances;
};
