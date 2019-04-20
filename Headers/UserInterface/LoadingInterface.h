#pragma once
#include "UserInterface/PromptInterface.h"

class LoadingInterface final : public PromptInterface
{
public:

    static LoadingInterface* getInstance();

    // Default Constructor
    LoadingInterface();
    LoadingInterface(const LoadingInterface* pCopy);                              // Default Copy Constructor
    LoadingInterface& operator=(const LoadingInterface* pCopy);                   // Assignment Operator.

    void updateOverride(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void renderOverride();

private:

    static LoadingInterface* m_pInstance;
};
