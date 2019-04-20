#pragma once
#include "UserInterface/PromptInterface.h"

class RulesInterface final : public PromptInterface
{
public:

    static RulesInterface* getInstance();

    // Default Constructor
    RulesInterface();
    RulesInterface(const RulesInterface* pCopy);                              // Default Copy Constructor
    RulesInterface& operator=(const RulesInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        VERSION,
    };

    static RulesInterface* m_pInstance;
};
