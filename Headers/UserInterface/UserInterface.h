#pragma once
#include "stdafx.h"

#define IMAGE_ROCKET "textures/hud/rocket.png"
#define IMAGE_TRAIL "textures/hud/trail.png"
#define IMAGE_EXPLOSION "textures/hud/explosion.png"
#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4

// x, y
#define UI_COMPONENT_COORDINATES 2
#define X 0
#define Y 1

// Forward Declaration
class ShaderManager;
class Texture;

/*
An interface to all user interface features.

Retrieves its values from GameStats

- Display text, images
*/
class UserInterface
{
public:

    enum eKillMessage
    {
        KILL_MESSAGE_FIRST_BLOOD = 0,
        KILL_MESSAGE_DOMINATION,
        KILL_MESSAGE_REVENGE,
        KILL_MESSAGE_KILLSTREAK,
        KILL_MESSAGE_KILL,
    };

    static UserInterface* getInstance(int iWidth, int iHeight);
    static UserInterface* getInstance();

    ~UserInterface();

    /*
    This visually updates the UserInterface to all value changes since last update.
    
    Under the scenes, this retrieves all needed the values from GameStats and
    displays them. This is why it does not need a time value in order to determine
    time-sensitive information such as cooldown and game time.
    @TODO this may change later on.
    
    This should be called once per frame update.
    
    */
    void update(float fSecondsSinceLastUpdate);

    void reinitialize(float gameTime);

    void render();

    void setDisplayCount(int count);

    void updateWidthAndHeight(int iWidth, int iHeight);

    void displayMessage(eHovercraft attacker, eHovercraft hit, eKillMessage message);

    // Display debug message
    // Set message to "" to disable debug message
    void displayDebug(const char* message);

private:

    // Used for m_vComponentScaling and m_vComponentCoordinates
    enum eUIComponent
    {
        COMPONENT_TIME = 0,
        COMPONENT_TRAIL,
        COMPONENT_SPIKES,
        COMPONENT_ROCKET,
        COMPONENT_DASH,
        COMPONENT_SCORE,
        COMPONENT_SCORE_CHANGE,
        COMPONENT_MESSAGE,
        COMPONENT_COUNT
    };

    UserInterface(int iWidth, int iHeight);                                 // Default Constructor
    UserInterface(const UserInterface* pCopy);                              // Default Copy Constructor
    UserInterface& operator=(const UserInterface* pCopy) {return (*this); } // Assignment Operator.
    static UserInterface* m_pInstance;

    // Initializes FreeType and the Font Library
    void initFreeType();
    void initializeVBOs();

    void setScore(int joystickID, int score);

    void displayMessage(eHovercraft hovercraft, std::string text);
    /*
    Other classes should not be able to directly tell the UI to render text or
    images. Instead, the UI gathers the necessary information from other
    classes, such as GameStats, where it decides what text and images needs to
    be updated during its update() call.
    */
    void renderComponent(eUIComponent component, GLfloat scale, vec3 color);
    void renderText(int text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void renderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
    void renderImage(string filepath, GLfloat x, GLfloat y, GLfloat scale);
    void initializeUserInterface();

    // Game Time
    void updateGameTime(float fSecondsSinceLastUpdate);
    void renderGameTime();
    
    // Message
    void renderMessages();

    // Score
    void initializeScores();
    void updateScores();
    void updateScore(eHovercraft hovercraft, int score);
    void renderScores();

    // Cooldowns
    void initializeCooldowns();
    void updateCooldowns();
    void renderCooldowns();
    void renderCooldown(std::string label, eCooldown cooldown, float* cooldowns, GLfloat x, GLfloat y, GLfloat scale);

    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        vec2    uvOffset;   // Offset to index the UV in the bitmap
        vec2    uvSize;     // Size of UVs for bitmap
        ivec2   size;       // Size of glyph
        ivec2   bearing;    // Offset from baseline to left/top of glyph
        GLuint  advance;    // Horizontal offset to advance to next glyph
    };
    map<GLchar, Character> m_pCharacters;
    void addNewCharacter(char c, const FT_GlyphSlotRec_* pGlyph, const vec2* vOffsets);
    void addBitmapToBuffer(const FT_Bitmap* pBitmap, char* cPtr);

    // VBO and VAO for rendering
    GLuint m_iVertexArray, m_iVertexBuffer, m_iTextureBuffer;

    /*
    NOTE: this may need to change in the future.

    Unit : seconds
    */
    float m_fGameTime;

    /*
    Tracks how long the message has been displayed for

    Unit : seconds
    */
    std::string m_sMessages[MAX_HOVERCRAFT_COUNT];
    float m_fMessageTimes[MAX_HOVERCRAFT_COUNT];

    /*
    Score updates appear temporarily just as messages are

    Unit : seconds
    */
    float m_fScoreChangeTimes[MAX_HOVERCRAFT_COUNT];

    int m_iDisplayCount;

    // Window reference
    int m_iWidth;
    int m_iHeight;

    // Determines how each UI component is scaled based on window dimensions
    // These values are empirically determined and are open to adjustment
    const float m_vComponentScaling[COMPONENT_COUNT][UI_COMPONENT_COORDINATES] =
    {
        // 0 Time
        {0.47f, 0.9f},
        // 1 Trail
        {0.2f, 0.3f},
        // 2 Spikes
        {0.2f, 0.2f},
        // 3 Rocket
        {0.7f, 0.3f},
        // 4 Dash
        {0.7f, 0.2f},
        // 5 Score
        {0.2f, 0.9f},
        // 6 Score Change
        {0.47f, 0.58f},
        // 7 Message
        {0.36f, 0.65f}
    };
    // Store the values so they do not need to be calculated every frame.
    float m_vComponentCoordinates[COMPONENT_COUNT][UI_COMPONENT_COORDINATES];

    // Singleton Pointers
    // The UI needs to render its own components
    ShaderManager *m_pShdrMngr;


    const char* debugMessage;
    float debugWidth;
    float debugHeight;
};
