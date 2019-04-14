#pragma once
#include "UserInterface/PromptInterface.h"

class SettingsInterface final : public PromptInterface
{
public:

    static SettingsInterface* getInstance(int iWidth, int iHeight);

    // Default Constructor
    SettingsInterface();
    SettingsInterface(const SettingsInterface* pCopy);                              // Default Copy Constructor
    SettingsInterface& operator=(const SettingsInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:
    enum eUIComponent
    {
        BACKGROUND = 0,
    };

    static SettingsInterface* m_pInstance;
};
