#include "UserInterface/FontManager.h"

// Singleton instance
FontManager* FontManager::m_pInstance = nullptr;

FontManager::FontManager()
{
    m_pTextureManager = TEXTURE_MANAGER->getInstance();
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
    m_pTextureManager = nullptr;
}

/*
Get a font of a specified name and size.

@param filename path to the font file
@param size     of the font
*/
//FTFont* FontManager::GetFont(const string *filename, int size)
//{
//  //  if ( fonts.find(*filename) != fonts.end())
//  //  {
//  //      // LOGMSG("Found font %s in list", filename);
//  //      return fonts[*filename];
//  //  }
//
//  //  FTFont* font = nullptr;
//  ////  FTFont* font = new FTTextureFont(filename->c_str());
//
//  //  if (!font->FaceSize(size))
//  //  {
//  //      // LOGERROR("Font %s failed to set size %i", filename, size);
//  //      delete font;
//  //      return NULL;
//  //  }
//
//  //  if (!font->Error())
//  //  {
//  //      fonts[*filename] = font;
//  //      return font;
//  //  }
//  //  return nullptr;
//}
