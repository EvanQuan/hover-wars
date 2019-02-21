#pragma once
#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"
/*


@author Evan Quan
*/
class FontManager
{
public:

    static FontManager* getInstance();

    ~FontManager();

   // FTFont* GetFont(const string *filename, int size);

private:
    // Hide these 'cause this is a singleton.
    FontManager();
    FontManager& operator= (const FontManager&) { return *this; };
    static FontManager* m_pInstance;

    TextureManager* m_pTextureManager;
};
