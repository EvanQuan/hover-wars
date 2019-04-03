#pragma once
#include "UserInterface/MenuInterface.h"

#define INSERT_COIN_OPTION "Insert Coin"

class RulesInterface final : public MenuInterface
{
public:

    static RulesInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    RulesInterface();
    RulesInterface(const RulesInterface* pCopy);                              // Default Copy Constructor
    RulesInterface& operator=(const RulesInterface* pCopy);                   // Assignment Operator.

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

    static RulesInterface* m_pInstance;

    void renderOption();
};
