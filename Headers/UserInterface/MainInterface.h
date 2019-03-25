#pragma once
#include "UserInterface/MenuInterface.h"

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
