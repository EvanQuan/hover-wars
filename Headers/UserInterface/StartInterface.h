#pragma once
#include "UserInterface/MenuInterface.h"


class StartInterface final : public MenuInterface
{
public:

    static StartInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    StartInterface(int iWidth, int iHeight);
    StartInterface(const StartInterface* pCopy);                              // Default Copy Constructor
    StartInterface& operator=(const StartInterface* pCopy) {return (*this); } // Assignment Operator.

    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        TITLE,
        INSERT_COIN
    };

    static StartInterface* m_pInstance;

    void renderOption(int choose);
};
