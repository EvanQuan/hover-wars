#pragma once
#include "UserInterface/MenuInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class AwardsInterface final : public MenuInterface
{
public:

    static AwardsInterface* getInstance(int iWidth, int iHeight);

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
        BACKGROUND = 0,
        TITLE,
        INSERT_COIN
    };

    static AwardsInterface* m_pInstance;

    void renderOption();
    void renderAwards();
    int compare(const void*a, const void*b);

};
