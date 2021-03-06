#pragma once
#include "UserInterface/PromptInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class ControlsInterface final : public PromptInterface
{
public:

    static ControlsInterface* getInstance();

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
