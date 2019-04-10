#pragma once
#include "UserInterface/MenuInterface.h"

#define NEW_GAME_OPTION "New Game"
#define GAME_RULE "Rules"
#define GAME_CONTROLLER "Controls"
#define QUIT_OPTION "Quit"


class MainInterface final : public MenuInterface
{
public:

    static MainInterface* getInstance(int iWidth, int iHeight);

    MainInterface();                                        // Default Constructor
    MainInterface(const MainInterface* pCopy);              // Default Copy Constructor
    MainInterface& operator=(const MainInterface* pCopy);   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:

    enum eUIComponent
    {
        BACKGROUND = 0,
        TITLE,
        NEW_GAME,
        RULE,
        CONTROLLER,
        EXIT
    };

    static MainInterface* m_pInstance;

    void renderOption();
};
