#include "UserInterface/FontManager.h"

// Singleton instance
FontManager* FontManager::m_pInstance = nullptr;

FontManager::FontManager()
{

}

/*
Get singletone instance.
*/
FontManager* FontManager::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new FontManager();
    }
    return m_pInstance;
}

FontManager::~FontManager()
{
    // Delete every font pointer in the font list
    map<string, FTFont*>::const_iterator font;
    for (font = fonts.begin(); font != fonts.end(); font++)
    {
        delete (*font).second;
    }

    fonts.clear();
}

/*
Get a font of a specified name and size.

@param filename path to the font file
@param size     of the font
*/
FTFont* FontManager::GetFont(const char *filename, int size)
{
    char buf[256];
    sprintf(buf, "%s%i", filename, size);
    string fontKey = string(buf);

    map<string, FTFont*>::const_iterator result = fonts.find(fontKey);
    if (result != fonts.end())
    {
        // LOGMSG("Found font %s in list", filename);
        return result->second;
    }

    FTFont* font = new FTTextureFont(filename);

    if (!font->FaceSize(size))
    {
        // LOGERROR("Font %s failed to set size %i", filename, size);
        delete font;
        return NULL;
    }

    if (!font->Error())

    fonts[fontKey] = font;

    return font;
}
