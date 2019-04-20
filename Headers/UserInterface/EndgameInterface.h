#pragma once
#include "UserInterface/PromptInterface.h"

class EndgameInterface final : public PromptInterface
{
public:

    static EndgameInterface* getInstance();

    // Default Constructor
    EndgameInterface();
    EndgameInterface(const EndgameInterface* pCopy);                              // Default Copy Constructor
    EndgameInterface& operator=(const EndgameInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        VERSION,
    };

    static EndgameInterface* m_pInstance;
};
