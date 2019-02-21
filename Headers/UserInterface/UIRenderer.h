#pragma once
#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"
/*


@author Evan Quan
*/
class UIRenderer
{
public:

    static UIRenderer* getInstance();

    ~UIRenderer();

   // FTFont* GetFont(const string *filename, int size);

private:
    // Hide these 'cause this is a singleton.
    UIRenderer();
    UIRenderer& operator= (const UIRenderer&) { return *this; };
    static UIRenderer* m_pInstance;

    TextureManager* m_pTextureManager;
};
