// ==========================================================================
// Image File Reading Support Functions
//  - requires the Magick++ development libraries: http://www.imagemagick.org
//  - or the FreeImage library: http://freeimage.sourceforge.net
//
// You may use this code (or not) however you see fit for your work.
//
// Add this header and the accompanying .cpp source file to your project, and
// ensure that you have one of the above libraries installed and the paths to
// it are set correctly.
//
// Authors: Sonny Chan, Alex Brown
//          University of Calgary
// Date:    January-February 2016
// ==========================================================================

#ifndef __IMAGE_READER_H__
#define __IMAGE_READER_H__

#include "stdafx.h"
#include "Texture.h"

// --------------------------------------------------------------------------
// Prototype for a function that loads an image with a given file name into
// an instance of the above structure. Returns true if successful.

bool InitializeTexture(Texture *texture, const std::string &imageFileName);

// --------------------------------------------------------------------------
#endif
