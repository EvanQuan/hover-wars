// ==========================================================================
// Image File Reading Support Functions
//  - requires the Magick++ development libraries: http://www.imagemagick.org
//  - or the FreeImage library: http://freeimage.sourceforge.net
//
// You may use this code (or not) however you see fit for your work.
//
// Set the #defines below to choose the image library you have installed on
// your system, that you will be using for your assignment. Then compile and
// link this source file with your project.
//
// Authors: Sonny Chan, Alex Brown
//          University of Calgary
// Date:    January-February 2016
// ==========================================================================
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ImageReader.h"

using namespace std;

// --------------------------------------------------------------------------
// FreeImage implementation of the InitializeTexture() function

bool InitializeTexture(Texture *mytex, const string &imageFileName)
{
    // Local Variables
    int iWidth, iHeight, nrComponents;
    bool bReturnValue = false;
    unsigned char* data = stbi_load(imageFileName.data(), &iWidth, &iHeight, &nrComponents, 0);

    // Ensure that data was properly loaded.
    if (nullptr != data)
    {
        // Get the format of the Texture
        GLenum eFormat;
        switch (nrComponents)
        {
        case 1:
            eFormat = GL_RED;
            break;
        case 3:
            eFormat = GL_RGB;
            break;
        case 4:
            eFormat = GL_RGBA;
            break;
        }

        // Generate Texture in GPU
        mytex->genTexture(data, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE);
        mytex->genMipMaps();
        mytex->setTexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        mytex->setTexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        bReturnValue = true;
    }
    else
        cout << "Failed to load Texture at path: " << imageFileName << endl;
        

    // Free the data that was loaded.
    stbi_image_free(data);

    return bReturnValue;
}

// --------------------------------------------------------------------------
