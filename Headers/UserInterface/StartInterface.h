#pragma once
#include "UserInterface/MenuInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class StartInterface final : public PromptInterface
{
public:

    static StartInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    StartInterface();
    StartInterface(const StartInterface* pCopy);                              // Default Copy Constructor
    StartInterface& operator=(const StartInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        TITLE,
        INSERT_COIN
    };

    static StartInterface* m_pInstance;

    void renderOption();
};
