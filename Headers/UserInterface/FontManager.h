#pragma once
#include "stdafx.h"

// FTTextureFont* myFont = FontManager::Instance().GetFont("arial.ttf", 72);

/*


@author Evan Quan
*/
class FontManager
{
public:

    static FontManager* getInstance();

    ~FontManager();

    FTFont* GetFont(const char *filename, int size);

private:
    // Hide these 'cause this is a singleton.
    FontManager();
    FontManager& operator= (const FontManager&) { return *this; };
    static FontManager* m_pInstance;

    // container for fonts
    map<string, FTFont*> fonts;
};
