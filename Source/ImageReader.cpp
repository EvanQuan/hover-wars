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

	width	= myImage.columns();
	height	= myImage.rows();

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

#include "FreeImage.h"

bool InitializeTexture(Texture *mytex, const string &imageFileName)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib = nullptr;
	char* bits = nullptr;
	GLuint height, width;

	fif = FreeImage_GetFileType(imageFileName.c_str(), 0);
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(imageFileName.c_str());
	if (fif == FIF_UNKNOWN)
		return false;

	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, imageFileName.c_str());
	if (!dib)
		return false;

	dib = FreeImage_ConvertTo32Bits( dib );

	bits = (char*)FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	if ((bits == 0) || (width == 0) || (height == 0))
		return false;

	mytex->genTexture( bits, width, height, GL_BGRA, GL_UNSIGNED_BYTE );

	FreeImage_Unload(dib);

	return true;
}

#endif
// --------------------------------------------------------------------------
