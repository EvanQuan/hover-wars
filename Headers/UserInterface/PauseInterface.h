#pragma once
#include "UserInterface/MenuInterface.h"

#define CONTINUE_OPTION "Continue"
#define END_GAME_OPTION "End Game"

class PauseInterface final : public MenuInterface
{
public:

    static PauseInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    PauseInterface();
    PauseInterface(const PauseInterface* pCopy);                              // Default Copy Constructor
    PauseInterface& operator=(const PauseInterface* pCopy);                   // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        CONTINUE,
        MAIN_MENU
    };

    static PauseInterface* m_pInstance;
    void renderOption();
};
