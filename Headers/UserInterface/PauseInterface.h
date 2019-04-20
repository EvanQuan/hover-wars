#pragma once
#include "UserInterface/PromptInterface.h"

#define CONTINUE_OPTION "Continue"
#define END_GAME_OPTION "End Game"

class PauseInterface final : public PromptInterface
{
public:

    static PauseInterface* getInstance();

    // Default Constructor
    PauseInterface();
    PauseInterface(const PauseInterface* pCopy);                              // Default Copy Constructor
    PauseInterface& operator=(const PauseInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
        CONTINUE,
        MAIN_MENU,
        PAUSER,
    };

    static PauseInterface* m_pInstance;
    void renderOption();
    void renderPauser();
};
