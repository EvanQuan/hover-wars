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
        TITLE,
        MUSIC_LABEL,
        MUSIC_SETTING,
        BACK,
    };

    static SettingsInterface* m_pInstance;

    void renderTitle();
    void renderMusic(bool selected, bool musicEnabled);
    void renderBack(bool selected);
};
