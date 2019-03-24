#pragma once
#include "UserInterface/UserInterface.h"


class MenuInterface abstract : public UserInterface
{
public:

    // Default Constructor
    MenuInterface(int iWidth, int iHeight,
        vector<pair<float, float>> componentScaling,
        vector<pair<float, float>> componentTranslating);
    MenuInterface(const MenuInterface* pCopy);                              // Default Copy Constructor
    MenuInterface& operator=(const MenuInterface* pCopy) {return (*this); } // Assignment Operator.

    virtual void update(float fSecondsSinceLastUpdate) = 0;

    virtual void reinitialize(float gameTime) = 0;

    virtual void render() = 0;
};
