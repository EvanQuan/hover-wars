#include "UserInterface/UserInterface.h"
#include "ShaderManager.h"
#include "TextureManager.h"

/***********\
 * Defines *
\***********/
#define FONT_HEIGHT             48                      // 48 Pixels High.
#define BITMAP_HEIGHT           512
#define BITMAP_WIDTH            512
#define MAX_ASCII_CHARS         255
#define GLYPH_BUFFER            1

/*
All user interface components have a location relative to the window dimensions
to ensure that the UI scales with window resizes.

Coordinate system:

(0, 1)                  (1, 1) 

(x=0, y=0)              (1, 0)
*/
#define TIME_SCALE              1.0f
#define SCORE_CHANGE_DURATION   1.0f
#define SCORE_CHANGE_SCALE      1.0f
#define SCORE_CHANGE_ADD_COLOR  COLOR_GREEN
#define SCORE_CHANGE_SUB_COLOR  COLOR_RED

#define MESSAGE_DURATION        3.0f
#define MESSAGE_SCALE           1.0f
#define MESSAGE_COLOR           COLOR_WHITE

// Unit: seconds
#define GLOBAL_MESSAGE_TIME 3.0f
// Translating offset to be as close to the bottom left corner as possible
// for large displays
#define GLOBAL_MESSAGE_X 5.0f
#define GLOBAL_MESSAGE_Y 10.0f


/*************\
 * Constants *
\*************/
const string ANDROID_FONT("fonts/Android.ttf");
const string ARIAL_FONT("fonts/arial.ttf");
const string VANADINE_FONT("fonts/Vanadine Bold.ttf");
const string ASTRON_BOY_REGULAR_FONT("fonts/astron-boy.regular.ttf");
const string FUTURE_OUTRUN_FONT("fonts/future.outrun-future.otf");

/*
The default font to use for the UI components
*/
const string  DEFAULT_FONT = ASTRON_BOY_REGULAR_FONT;

const float F_BITMAP_HEIGHT = static_cast<float>(BITMAP_HEIGHT);
const float F_BITMAP_WIDTH = static_cast<float>(BITMAP_WIDTH);

UserInterface::UserInterface(vector<pair<float, float>> componentScaling,
                              vector<pair<float, float>> componentTranslating)
{
    // Get Singleton Handles
    m_pShdrMngr = SHADER_MANAGER;

    m_vComponentScaling = componentScaling;
    m_vComponentTranslating = componentTranslating;
    m_iComponentCount = componentScaling.size();

    initializeComponentCoordinates();

    initFreeType();
    initializeVBOs();

    m_Textures = TEXTURE_MANAGER->loadTextures(m_vTextureFiles, IMAGE_DIRECTORY);
}

UserInterface::~UserInterface()
{
    m_pShdrMngr = nullptr;

    // Clean up VBO and VAO
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteTextures(1, &m_iTextureBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);
}

void UserInterface::initializeComponentCoordinates()
{
    for (int i = 0; i < m_iComponentCount; i++)
    {
        m_vComponentCoordinates.push_back(make_pair(0.0f, 0.0f));
    }
}

void UserInterface::renderGlobalMessage()
{
    if (m_fGlobalMessageTime > 0)
    {
        renderText(m_sGlobalMessage, m_fGlobalMessageX, m_fGlobalMessageY, 1.0, COLOR_WHITE);
    }
}

// Initializes The FreeType Library and Fonts as well as any other Characters necessary for displaying UI
void UserInterface::initFreeType()
{
    // Local Variables
    bool bLoaded = true;                // Determines if the Font was properly loaded or not.
    FT_Library  ftLibrary;              // FreeType Data Structures
    FT_Face     ftFace;
    GLsizei fHeight(0), fWidth(0);      // Height and Width Trackers for generating the Bitmap
    vec2 vOffsets = vec2(0.0f);         // Compute Offsets for UVs every entry.
    char cData[BITMAP_WIDTH * BITMAP_HEIGHT] = { '\0' };   // character array buffer for storing bitmap.
    char* cPtr = cData;                 // Pointer for inserting Data into the buffer.

    // Initialize FreeType Library and Default Font: Each function returns a non-zero integer when an error occurs.
    bLoaded = (0 == FT_Init_FreeType(&ftLibrary));
    if (!bLoaded )
        cout << "ERROR: Freetype: could not initialize FreeType Library for UI.\n";

    // Initialize Default Face
    bLoaded &= (0 == FT_New_Face(ftLibrary, DEFAULT_FONT.c_str(), 0, &ftFace));
    if (!bLoaded )
        cout << "ERROR: Freetype: failed to load \"" << DEFAULT_FONT << "\" for UI.\n";

    // Successfully loaded -> Finish loading the rest of the Library.
    if (bLoaded)
    {
        FT_Set_Pixel_Sizes(ftFace, 0, FONT_HEIGHT);   // Set the Font Width to 0 to automatically adjust width based on height.

        for (GLubyte c = 0; c < MAX_ASCII_CHARS; ++c)
        {
            // Load Character Glyph
            if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
            {
                cout << "ERROR: FreeType: Failed to load Glyph (\'" << c << "\')\n";
                continue;
            }
            else if (ftFace->glyph->bitmap.buffer != nullptr)
            {
                // Concat the resulting bitmap to a specified width.
                if (fWidth + ftFace->glyph->bitmap.width + GLYPH_BUFFER > BITMAP_WIDTH)
                {
                    fWidth = 0;             // Overflow? reset Width to 0 for next line
                    vOffsets.x = 0.0f;      // Set Offset x back to 0
                    vOffsets.y += static_cast<float>(fHeight) + GLYPH_BUFFER;  // Set Offset y to next row
                    cPtr = cData + (BITMAP_WIDTH * static_cast<unsigned int>(vOffsets.y)); // Reset pointer to next line in buffer.
                    fHeight = 0;            // Reset Height for new pass.
                }

                // Probe to see how far the bitmap will write into the buffer.
                char* cTest = (cPtr + ftFace->glyph->bitmap.width + GLYPH_BUFFER + (BITMAP_WIDTH * ftFace->glyph->bitmap.rows));

                // If it won't go out of bounds, save to buffer
                if (cTest - cData < (BITMAP_HEIGHT * BITMAP_WIDTH))
                    addBitmapToBuffer(&ftFace->glyph->bitmap, cPtr);
                else    // If it will go out of bounds, escape and input an error.
                {
                    cout << "ERROR: FreeType: Could not construct proper Bitmap, Buffer went out of bounds.\n";
                    break;
                }

                // Fast Forward to next Glyph position in buffer.
                cPtr += ftFace->glyph->bitmap.width + GLYPH_BUFFER;
            }

            // Store Character in CharacterMap
            addNewCharacter(c, ftFace->glyph, &vOffsets);

            // Update tracking information for bitmaps.
            vOffsets.x += ftFace->glyph->bitmap.width + GLYPH_BUFFER;
            fWidth += ftFace->glyph->bitmap.width + GLYPH_BUFFER;
            fHeight = (ftFace->glyph->bitmap.rows > static_cast<unsigned int>(fHeight) ? ftFace->glyph->bitmap.rows : fHeight);
        }

        // Generate Bitmap Texture
        glGenTextures(1, &m_iTextureBuffer);
        glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, BITMAP_WIDTH, BITMAP_HEIGHT,
            0, GL_RED, GL_UNSIGNED_BYTE, cData);
        
        // Set Texture Options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    // Clean up FreeType after finished processing the glyphs
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLibrary);
}

// Given a FreeType Bitmap, add the buffer to a character Buffer at the given pointer.
//  This function is private and should only be called after ensuring that the bitmap will not write out of
//  the bounds of the character buffer.
void UserInterface::addBitmapToBuffer(const FT_Bitmap* pBitmap, char* cPtr)
{
    // Add Glyph Bitmap to larger Bitmap
    for (unsigned int y = 0; y < pBitmap->rows; ++y) // Iterate through rows
    {
        for (unsigned int x = 0; x < pBitmap->width; ++x) // Iterate through pixels within row
        {
            // Add the pixel to the buffer at the proper index.
            *(cPtr++) = pBitmap->buffer[(y * pBitmap->width) + x];
        }
        // Jump to the next row in the data buffer, offset by the glyph width for a new row.
        cPtr += BITMAP_WIDTH - pBitmap->width;
    }
}

// Generates a New Character struct for a given character and associated glyph, then adds the entry into
//  the character map.
void UserInterface::addNewCharacter(char c, const FT_GlyphSlotRec_* pGlyph, const vec2* vOffsets)
{
    // Generate Texture - Deal with Textures locally for simplicy as they don't need to be generalized yet.
    Character cNewChar;

    // Set up rest of Character Structure
    cNewChar.uvOffset = vec2(vOffsets->x / F_BITMAP_WIDTH,                                      // UV Offset for Bitmap
                             vOffsets->y / F_BITMAP_HEIGHT);
    cNewChar.uvSize = vec2(static_cast<float>(pGlyph->bitmap.width) / F_BITMAP_WIDTH,    // Size of UV section
                           static_cast<float>(pGlyph->bitmap.rows) / F_BITMAP_HEIGHT);
    cNewChar.size = ivec2(pGlyph->bitmap.width, pGlyph->bitmap.rows);     // Pixel Size
    cNewChar.bearing = ivec2(pGlyph->bitmap_left, pGlyph->bitmap_top);    // Bearing Information
    cNewChar.advance = pGlyph->advance.x;                                        // Glyph Advance information

    // Store Character for Later use.
    m_pCharacters.insert(make_pair(c, cNewChar));
}

void UserInterface::initializeVBOs()
{
    // Generate Vertex Array
    glGenVertexArrays(1, &m_iVertexArray);
    // dynamic draw, keep this ready, because we will draw this alot
    m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, nullptr, sizeof(vec4), GL_DYNAMIC_DRAW);    // Generate the Vertex Buffer and store some space on the GPU for Text Rendering.

    // chunk size  4 - internally treats each section as a float, it goes through 4 floats in each cunk
    // 
    // offset is 0
    m_pShdrMngr->setAttrib(m_iVertexArray, 0, 4, sizeof(vec4), 0); // Set Attributes for the Buffer to let OpenGL know how to index the data.
}

void UserInterface::render()
{
    renderOverride();
    renderGlobalMessage();
}

/*
Update the window width and height due to window resizing.
*/
void UserInterface::updateWidthAndHeight(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    // Update UI Projection Matrix Uniform for UI Shader
    mat4 m4UIProjection = ortho(0.0f, static_cast<GLfloat>(m_iWidth), 0.0f, static_cast<GLfloat>(m_iHeight), 0.0f, 1.0f);
    m_pShdrMngr->setUnifromMatrix4x4(ShaderManager::eShaderType::UI_SHDR, "UIProjection", &m4UIProjection);

    // Update and store all component coordinates
    for (int component = 0; component < m_iComponentCount; component++)
    {
        m_vComponentCoordinates[component].first = (m_iWidth * m_vComponentScaling[component].first) + m_vComponentTranslating[component].first;
        m_vComponentCoordinates[component].second = (m_iHeight * m_vComponentScaling[component].second) + m_vComponentTranslating[component].second;
    }
    // Update global message coordinates
    m_fGlobalMessageX = GLOBAL_MESSAGE_X;
    m_fGlobalMessageY = GLOBAL_MESSAGE_Y;

}

void UserInterface::renderText(int text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
    renderText(std::to_string(text), x, y, scale, color);
}
/*
Render text to the screen.

Window coordinates in pixels
(0, height)     (width, height)


(0, 0)          (width, 0)

@param text     to render
@param x        x-coordinate of the bottom-left corner of text, in pixels
@param y        y-coordinate of the bottom-left corner of text, in pixels
@param scale    text, where 1.0 is the default size
@param color    rgb colors of the text
*/
void UserInterface::renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
    // Vector for storing VBO data.
    vector<vec4> vTextOutput;

    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    m_pShdrMngr->setUniformVec3(ShaderManager::eShaderType::UI_SHDR, "textColor", &color);
    // Is text, not an image. This distinguishment needs to be made since images and
    // text share the same shader.
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "isImage", false);
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "backgroundImage", false);

    // Bind Texture.
    glActiveTexture(GL_TEXTURE0 + m_iTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
    SHADER_MANAGER->setUniformInt(ShaderManager::eShaderType::UI_SHDR, "text", m_iTextureBuffer);

    // Iterate through all Characters
    string::const_iterator c;
    for (c = text.begin(); c != text.end(); ++c)
    {
        // Grab Character Glyph information
        Character ch = m_pCharacters[*c];

        // Calculate Position offset by the bearings of the glyph
        // x and y in screen space
        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        // Calculate width and height of the glyph
        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Generate Information for 4 Corners for readability and less computation
        vec4 vCorners[4] = {
            vec4(xpos,      ypos,       ch.uvOffset.x,                  ch.uvOffset.y + ch.uvSize.y),
            vec4(xpos + w,  ypos,       ch.uvOffset.x + ch.uvSize.x,    ch.uvOffset.y + ch.uvSize.y),
            vec4(xpos,      ypos + h,   ch.uvOffset.x,                  ch.uvOffset.y),
            vec4(xpos + w,  ypos + h,   ch.uvOffset.x + ch.uvSize.x,    ch.uvOffset.y)
        };

        // Triangles
        /*
            
            2
            |\  
            | \ 
            |  \
            0---1
        */
        vTextOutput.push_back(vCorners[BOTTOM_LEFT]);
        vTextOutput.push_back(vCorners[BOTTOM_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_LEFT]);

        // Triangles
        /*

            2---3
             \  |
              \ |
               \|
                1
        */
        vTextOutput.push_back(vCorners[TOP_LEFT]);
        vTextOutput.push_back(vCorners[BOTTOM_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_RIGHT]);

        // Now advance the cursors for next glyph (note: advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // >> 6 == 1/64 (2^6 = 64)
    }

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vTextOutput.size() * sizeof(vec4), vTextOutput.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render Quad
    glDrawArrays(GL_TRIANGLES, 0, vTextOutput.size());

    // Clean up OpenGL
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UserInterface::renderText(const string text, int component, vec3 color)
{
    renderText(text, component, 1.0f, 1.0f, color);
}

// Shows the Sprite Map for the text map for debugging.
void UserInterface::debugFont()
{
    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    // Is text, not an image. This distinguishment needs to be made since images and
    // text share the same shader.
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "isImage", false);

    // Bind Texture.
    glActiveTexture(GL_TEXTURE0 + m_iTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, m_iTextureBuffer);
    SHADER_MANAGER->setUniformInt(ShaderManager::eShaderType::UI_SHDR, "text", m_iTextureBuffer);

    vec4 vCorners[4] = {
        vec4(0.0f,  0.0f,  0.0f, 1.0f),
        vec4(10.0f, 0.0f,  1.0f, 1.0f),
        vec4(0.0f,  10.0f, 0.0f, 0.0f),
        vec4(10.0f, 10.0f, 1.0f, 0.0f)
    };

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4) << 2), vCorners, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render Quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Clean up OpenGL
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UserInterface::displayGlobalMessage(const string message)
{
    m_sGlobalMessage = message;
    m_fGlobalMessageTime = GLOBAL_MESSAGE_TIME;
}

void UserInterface::renderText(const string text, int component, float x, float y, vec3 color)
{
    renderText(text,
        m_vComponentCoordinates[component].first + x,
        m_vComponentCoordinates[component].second + y,
        1.0f,
        color);
}

/*
TODO Use hashmap for image instead of image filepath directly
Render image to the screen.

Window coordinates in pixels
(0, height)     (width, height)


(0, 0)          (width, 0)

@param filepath of image
@param x        x-coordinate of the bottom-left corner of text, in pixels
@param y        y-coordinate of the bottom-left corner of text, in pixels
@param scale    image, where 1.0 is the default size
*/
void UserInterface::renderImage(const string filepath, GLfloat x, GLfloat y, GLfloat scale)
{
    // Get texture height and width
    auto tFoundIt = m_Textures.find(filepath);
    Texture* image = tFoundIt->second;
    int iImage_x, iImage_y;
    image->getTextureDimensions(&iImage_y, &iImage_x);

    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    // Is an image, not text. This distinguishment needs to be made since images and
    // text share the same shader.
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "isImage", true);
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "backgroundImage", false);

    vec4 vCorners[4] = {
    vec4(x           ,            y, 0.0f, 1.0f),
    vec4(x + iImage_x,            y, 1.0f, 1.0f),
    vec4(x           , y + iImage_y, 0.0f, 0.0f),
    vec4(x + iImage_x, y + iImage_y, 1.0f, 0.0f)
    };

    image->bindTexture(ShaderManager::eShaderType::UI_SHDR, "text");

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4) << 2), vCorners, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render Quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    image->unbindTexture();
}

void UserInterface::renderImage(const string filepath, int component)
{
    renderImage(filepath, component, 0, 0);
}

void UserInterface::renderImage(const string filepath, int component, float x, float y)
{
    renderImage(filepath,
        m_vComponentCoordinates[component].first + x,
        m_vComponentCoordinates[component].second + y,
        1.0f);
}

void UserInterface::renderBackgroundImage(const string filepath)
{
    // Get texture height and width
    auto tFoundIt = m_Textures.find(filepath);
    Texture* image = tFoundIt->second;
    int iImage_x, iImage_y;
    image->getTextureDimensions(&iImage_y, &iImage_x);

    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    // Is an image, not text. This distinguishment needs to be made since images and
    // text share the same shader.
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "isImage", true);
    m_pShdrMngr->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "backgroundImage", true);

    vec4 vCorners[4] = {
        vec4(-1.0f, -1.0f, 0.0f, 1.0f), /*Bottom Left*/
        vec4(1.0f,  -1.0f, 1.0f, 1.0f), /*Bottom Right*/
        vec4(-1.0f, 1.0f,  0.0f, 0.0f), /*Top Left*/
        vec4(1.0f,  1.0f,  1.0f, 0.0f), /*Top Right*/
    };

    image->bindTexture(ShaderManager::eShaderType::UI_SHDR, "text");

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4) << 2), vCorners, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render Quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    image->unbindTexture();
}


