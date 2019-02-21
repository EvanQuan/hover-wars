#include "UserInterface/UIRenderer.h"

// Singleton instance
UIRenderer* UIRenderer::m_pInstance = nullptr;

UIRenderer::UIRenderer()
{
    m_pTextureManager = TEXTURE_MANAGER->getInstance();
}

/*
Get singletone instance.
*/
UIRenderer* UIRenderer::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new UIRenderer();
    }
    return m_pInstance;
}

UIRenderer::~UIRenderer()
{
    m_pTextureManager = nullptr;
}

/*
Get a font of a specified name and size.

@param filename path to the font file
@param size     of the font
*/
//FTFont* UIRenderer::GetFont(const string *filename, int size)
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
