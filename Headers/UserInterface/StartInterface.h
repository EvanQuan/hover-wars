#pragma once
#include "UserInterface/PromptInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class StartInterface final : public PromptInterface
{
public:

    static StartInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    StartInterface();
    StartInterface(const StartInterface* pCopy);                              // Default Copy Constructor
    StartInterface& operator=(const StartInterface* pCopy);                   // Assignment Operator.

    void reinitialize(float gameTime);


private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        TITLE,
        INSERT_COIN
    };

    static StartInterface* m_pInstance;

    void updateOverride(float fTimeInSeconds);

    void renderOverride();

    void renderOption();

    float m_fBlinkTime;
    bool m_bPromptVisible;
};
