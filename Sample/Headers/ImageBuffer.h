// ==========================================================================
// Pixel Rendering and Image Saving Support Code
//  - requires the OpenGL Mathmematics (GLM) library: http://glm.g-truc.net
//  - requires the Magick++ development libraries: http://www.imagemagick.org
//  -   or the FreeImage library: http://freeimage.sourceforge.net
//
// You may use this code (or not) however you see fit for your work.
//
// Authors: Sonny Chan, Alex Brown
//          University of Calgary
// Date:    February-March 2016
// ==========================================================================
#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include "stdafx.h"
#include <glm/vec3.hpp>
#include <vector>

#ifndef GLFW_VERSION_MAJOR
#ifdef USING_LINUX
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>
#endif

// --------------------------------------------------------------------------
// This class encapsulates functionality for setting pixel colours in an
// image memory buffer, copying the buffer into an OpenGL window for display,
// and saving the buffer to disk as an image file.

class ImageBuffer
{
    // OpenGL texture corresponding to our image, and an FBO to render it
    GLuint  m_textureName;
    GLuint  m_framebufferObject;

    // dimensions of our image, and the pixel colour data array
    int     m_width, m_height;
    std::vector<glm::vec3> m_imageData;

    // state variables to keep track of modified region
    bool    m_modified;
    int     m_modifiedLower, m_modifiedUpper;

    void ResetModified();

public:
    ImageBuffer();
    ~ImageBuffer();

    // returns the width or height of the currently allocated image
    int Width() const  { return m_width; }
    int Height() const { return m_height; }

    // call this after your OpenGL context is all set up to create an image
    // buffer that matches the size of your viewport
    bool Initialize();

    // set a pixel in this image buffer to a specified colour:
    //  - (0,0) is the bottom-left pixel of the image
    //  - colour is RGB given as floating point numbers in the range [0,1]
    void SetPixel(int x, int y, glm::vec3 colour);

    // call this in your render function to copy this image onto your screen
    void Render();

    // call this at the end of your render to save the image to file
    bool SaveToFile(const std::string &imageFileName);
};

// --------------------------------------------------------------------------
#endif // IMAGEBUFFER_H
