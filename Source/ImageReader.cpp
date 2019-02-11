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

#include "EnvSpec.h"
#include "ImageReader.h"

using namespace std;

// --------------------------------------------------------------------------
// ImageMagick implementation of the InitializeTexture() function
#ifdef USING_LINUX

#include <Magick++.h>

bool InitializeTexture(Texture *texture, const string &imageFileName)
{
    Magick::Image myImage;
    GLuint width, height;
    
    // try to read the provided image file
    try {
        myImage.read(imageFileName);
    }
    catch (Magick::Error &error) {
        cout << "Magick++ failed to read image " << imageFileName << endl;
        cout << "ERROR: " << error.what() << endl;
        return false;
    }

    width    = myImage.columns();
    height    = myImage.rows();

    // create a Magick++ pixel cache from the image for direct access to data
    Magick::Pixels pixelCache(myImage);
    Magick::PixelPacket *pixels;
    pixels = pixelCache.get(0, 0, width, height);
    
    // determine the number of stored bytes per pixel channel in the cache
    GLenum channelDataType;
    switch (sizeof(Magick::Quantum)) {
        case 4:     channelDataType = GL_UNSIGNED_INT;      break;
        case 2:     channelDataType = GL_UNSIGNED_SHORT;    break;
        default:    channelDataType = GL_UNSIGNED_BYTE;
    }

    texture->genTexture( pixels, width, height, GL_BGRA, channelDataType );

    return true;
}

#endif
// --------------------------------------------------------------------------
// FreeImage implementation of the InitializeTexture() function
#ifdef USING_WINDOWS

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
        
        bReturnValue = true;
    }
    else
        cout << "Failed to load Texture at path: " << imageFileName << endl;
        

    // Free the data that was loaded.
    stbi_image_free(data);

    return bReturnValue;
}

#endif
// --------------------------------------------------------------------------
