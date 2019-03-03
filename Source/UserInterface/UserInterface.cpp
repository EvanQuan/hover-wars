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
#define TOP_LEFT                2
#define BOTTOM_LEFT             0
#define BOTTOM_RIGHT            1
#define TOP_RIGHT               3

/*************\
 * Constants *
\*************/
const string ANDROID_FONT("fonts/Android.ttf");
const string ARIAL_FONT("fonts/arial.ttf");
const float F_BITMAP_HEIGHT = static_cast<float>(BITMAP_HEIGHT);
const float F_BITMAP_WIDTH = static_cast<float>(BITMAP_WIDTH);

// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface(int iWidth, int iHeight)
{
    // Get Singleton Handles
    m_pGameStats = GameStats::getInstance();
    m_pShdrMngr = SHADER_MANAGER;

    m_iDisplayCount = 0;
    updateWidthAndHeight(iWidth, iHeight);

    initializeUserInterface();
    initFreeType();
    initializeVBOs();
}

UserInterface* UserInterface::getInstance(int iWidth, int iHeight)
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new UserInterface(iWidth, iHeight);
    }
    return m_pInstance;
}

/*
Assumes already instantiated beforehand.
*/
UserInterface* UserInterface::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

UserInterface::~UserInterface()
{
    m_pGameStats = nullptr;
    m_pShdrMngr = nullptr;

    // Clean up VBO and VAO
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteTextures(1, &m_iTextureBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);
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
    bLoaded &= (0 == FT_New_Face(ftLibrary, ARIAL_FONT.c_str(), 0, &ftFace));
    if (!bLoaded )
        cout << "ERROR: Freetype: failed to load \"" << ARIAL_FONT << "\" for UI.\n";

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
                if (fWidth + ftFace->glyph->bitmap.width > BITMAP_WIDTH)
                {
                    fWidth = 0;             // Overflow? reset Width to 0 for next line
                    vOffsets.x = 0.0f;      // Set Offset x back to 0
                    vOffsets.y += static_cast<float>(fHeight);  // Set Offset y to next row
                    cPtr = cData + (BITMAP_WIDTH * static_cast<unsigned int>(vOffsets.y)); // Reset pointer to next line in buffer.
                    fHeight = 0;            // Reset Height for new pass.
                }

                // Probe to see how far the bitmap will write into the buffer.
                char* cTest = (cPtr + ftFace->glyph->bitmap.width + (BITMAP_WIDTH * ftFace->glyph->bitmap.rows));

                // If it won't go out of bounds, save to buffer
                if (cTest - cData < (BITMAP_HEIGHT * BITMAP_WIDTH))
                    addBitmapToBuffer(&ftFace->glyph->bitmap, cPtr);
                else    // If it will go out of bounds, escape and input an error.
                {
                    cout << "ERROR: FreeType: Could not construct proper Bitmap, Buffer went out of bounds.\n";
                    break;
                }

                // Fast Forward to next Glyph position in buffer.
                cPtr += ftFace->glyph->bitmap.width;
            }

            // Store Character in CharacterMap
            addNewCharacter(c, ftFace->glyph, &vOffsets);

            // Update tracking information for bitmaps.
            vOffsets.x += ftFace->glyph->bitmap.width;
            fWidth += ftFace->glyph->bitmap.width;
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
    m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, nullptr, sizeof(vec4), GL_DYNAMIC_DRAW);    // Generate the Vertex Buffer and store some space on the GPU for Text Rendering.
    m_pShdrMngr->setAttrib(m_iVertexArray, 0, 4, sizeof(vec4), 0); // Set Attributes for the Buffer to let OpenGL know how to index the data.
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
}

/*
This visually updates the UserInterface to all value changes since last update.

Under the scenes, this retrieves all needed the values from GameStats and
displays them. This is why it does not need a time value in order to determine
time-sensitive information such as cooldown and game time.
@TODO this may change later on.

This should be called once per frame update.

*/
void UserInterface::update(float fSecondsSinceLastUpdate)
{
    if (m_iDisplayCount > 0)
    {
        // system("CLS");
        updateGameTime(fSecondsSinceLastUpdate);
        updateScores();
        updateCooldowns();
    }
}

/*

*/
void UserInterface::updateGameTime(float fSecondsSinceLastUpdate)
{

}

/*
Update the scores for all display count players
*/
void UserInterface::updateScores()
{
    for (int player = 0; player < m_iDisplayCount; player++)
    {
        updateScore((ePlayer) player, m_pGameStats->get((ePlayer) player, GameStats::CURRENT_SCORE));
    }
}

void UserInterface::updateScore(ePlayer player, int score)
{
    // cout << "Player " << (player + 1) << " score: " << score << endl;
}

void UserInterface::updateCooldowns()
{
    // renderImage(IMAGE_TRAIL, 0, 0, 10);
}

void UserInterface::renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
    // Vector for storing VBO data.
    vector<vec4> vTextOutput;

    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(ShaderManager::eShaderType::UI_SHDR));
    m_pShdrMngr->setUniformVec3(ShaderManager::eShaderType::UI_SHDR, "textColor", &color);

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

        // Update VBO for each character
        // Triangle 1:
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

        // Triangle 2
        /*
            2---1
             \  |
              \ |
               \|
                0
        */
        vTextOutput.push_back(vCorners[BOTTOM_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_RIGHT]);
        vTextOutput.push_back(vCorners[TOP_LEFT]);

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

void UserInterface::renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale)
{


}

/*
Set the number of UIs to display.
Values:
    0 - No UIs are displayed. This should be done in the main menu (not in game).
    1 - 1 player. This will display across the whole window.
    2 - 2 player. Each UI will display across 1/4 of the window.
    3 - 3 player. Each UI will display across 1/4 of the window.
    4 - 4 player. Each UI will display across 1/4 of the window.

@TODO set count for main menu or pause menu
*/
void UserInterface::setDisplayCount(int count)
{
    if (DISPLAY_COUNT_MIN > count)
    {
        count = DISPLAY_COUNT_MIN;
    }
    else if (DISPLAY_COUNT_MAX < count)
    {
        count = DISPLAY_COUNT_MAX;
    }
    m_iDisplayCount = count;

    initializeUserInterface();
}

/*
Initialize all aspects of the UI according to the current display count. This
should be done at the start of every game, or if the game resets.
*/
void UserInterface::initializeUserInterface()
{
    initializeScores();
    initializeCooldowns();
}

/*
Initialize the scores of all players to 0
*/
void UserInterface::initializeScores()
{
    for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
    {
    }
}

/*
Initialize the cool down counts for all abilities
*/
void UserInterface::initializeCooldowns()
{
    for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
    {
        // TODO
    }
}

/*
Set a specified player's score to the specified value.
*/
void UserInterface::setScore(int joystickID, int score)
{
}

/*
Initialize all UI components according to the current value of m_iDisplayCount
*/
void initializeDisplayCount()
{
    // TODO
}
