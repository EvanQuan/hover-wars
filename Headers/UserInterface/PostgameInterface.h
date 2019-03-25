#pragma once
#include "UserInterface/MenuInterface.h"

#define BACK_TO_MAIN_MENU_OPTION "Return to Main Menu"

class PostgameInterface final : public MenuInterface
{
public:

    static PostgameInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PostgameInterface(int iWidth, int iHeight);
    PostgameInterface(const PostgameInterface* pCopy);                              // Default Copy Constructor
    PostgameInterface& operator=(const PostgameInterface* pCopy) {return (*this); } // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:

    enum eUIComponent
    {
        BACKGROUND = 0,
        MAIN_MENU,
    };
    static PostgameInterface* m_pInstance;

    void renderOption();
    void renderScore();
};
