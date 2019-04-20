#pragma once
#include "UserInterface/PromptInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class AwardsInterface final : public PromptInterface
{
public:

    static AwardsInterface* getInstance();

    // Default Constructor
    AwardsInterface();
    AwardsInterface(const AwardsInterface* pCopy);                              // Default Copy Constructor
    AwardsInterface& operator=(const AwardsInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        TITLE,
    };

    static AwardsInterface* m_pInstance;

    void renderOption();
    void renderAwards();
};
