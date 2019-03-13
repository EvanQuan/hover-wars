#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "InputHandler.h"
#include "SoundManager.h"

#ifndef NDEBUG
    #include "UserInterface/UserInterface.h"
#endif

/* CLASS */
/*
Menus determine the state of the game with respect to how input is processed by
the CommandHandler.

Depending on the current menu state of the CommandHander, the input is processed
differently.

Every subclass should add its singleton instance to CommandHandler's
menuInstances vector.

@author Evan Quan
*/
class Menu abstract
{
public:
    ~Menu();

    virtual void update() = 0;

private:
    bool bWireFrameEnabled;
protected:
    Menu();

    void nextMenu(Menu* next);

    InputHandler *m_pInputHandler;

    // Only player hovercrafts receive input from the Menu.
    // Bots receive input from their AI components.
    std::vector<HovercraftEntity*> players;

    // Toggles Wireframe drawing
    void Menu::debugToggleWireframe();

};
