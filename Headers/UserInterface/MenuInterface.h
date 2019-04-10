#pragma once
#include "UserInterface/UserInterface.h"


class MenuInterface abstract : public UserInterface
{
public:

    // Default Constructor
    MenuInterface(vector<pair<float, float>> componentScaling,
                  vector<pair<float, float>> componentTranslating);
    MenuInterface(const MenuInterface* pCopy);                              // Default Copy Constructor
    MenuInterface& operator=(const MenuInterface* pCopy);                   // Assignment Operator.

    virtual void updateOverride(float fSecondsSinceLastUpdate) = 0;

    virtual void reinitialize(float gameTime) = 0;

    virtual void render() = 0;
};
