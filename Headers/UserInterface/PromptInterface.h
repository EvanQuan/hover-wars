#pragma once
#include "UserInterface/UserInterface.h"


/*
    PromptInterfaces visually correspond with PromptMenus.
*/
class PromptInterface abstract : public UserInterface
{
public:

    // Default Constructor
    PromptInterface(vector<pair<float, float>> componentScaling,
                  vector<pair<float, float>> componentTranslating);
    PromptInterface(const PromptInterface* pCopy);                              // Default Copy Constructor
    PromptInterface& operator=(const PromptInterface* pCopy);                   // Assignment Operator.

    virtual void updateOverride(float fSecondsSinceLastUpdate) = 0;

    virtual void reinitialize(float gameTime) = 0;

    virtual void renderOverride() = 0;
};
