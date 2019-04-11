#pragma once
#include "UserInterface/MenuInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class ControlsInterface final : public MenuInterface
{
public:

    static ControlsInterface* getInstance(int iWidth, int iHeight);

    ControlsInterface();                                            // Default Constructor
    ControlsInterface(const ControlsInterface* pCopy);              // Default Copy Constructor
    ControlsInterface& operator=(const ControlsInterface* pCopy);   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
    };

    static ControlsInterface* m_pInstance;
};
