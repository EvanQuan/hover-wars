#pragma once
#include "UserInterface/MenuInterface.h"


#define BACKGROUND "textures/menu/main_menu.png"
#define NEW_GAME_1 "textures/menu/new_game.png"
#define NEW_GAME_2 "textures/menu/new_game2.png"
#define EXIT_1 "textures/menu/exit.png"
#define EXIT_2 "textures/menu/exit2.png"

class MainInterface final : public MenuInterface
{
public:

    static MainInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    MainInterface(int iWidth, int iHeight);
    MainInterface(const MainInterface* pCopy);                              // Default Copy Constructor
    MainInterface& operator=(const MainInterface* pCopy) {return (*this); } // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:

    enum eUIComponent
    {
        COMPONENT_BACKGROUND = 0,
        COMPONENT_NEW_GAME,
        COMPONENT_EXIT
    };

    static MainInterface* m_pInstance;

    void renderOption(int choice);
};
