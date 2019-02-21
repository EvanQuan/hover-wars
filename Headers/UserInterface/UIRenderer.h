#pragma once
#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"
/*
Renders all stuff for 1 User Interface display

@author Evan Quan
*/
class UIRenderer
{
public:

    static UIRenderer* getInstance(GLFWwindow* window);


    ~UIRenderer();

   // FTFont* GetFont(const string *filename, int size);

private:
    // Hide these 'cause this is a singleton.
    UIRenderer();
    UIRenderer& operator= (const UIRenderer&) { return *this; };
    static UIRenderer* m_pInstance;

    TextureManager* m_pTextureManager;
};
