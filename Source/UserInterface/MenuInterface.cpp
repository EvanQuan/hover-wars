#include "UserInterface/MenuInterface.h"

MenuInterface::MenuInterface(int iWidth, int iHeight,
    vector<pair<float, float>> componentScaling,
    vector<pair<float, float>> componentTranslating)
    : UserInterface(iWidth, iHeight,
        componentScaling, componentTranslating)
{
    /* TODO */
}

void MenuInterface::render()
{
    renderImage("textures/menu/main_menu.png", 0, 0, 1.0f);
}
