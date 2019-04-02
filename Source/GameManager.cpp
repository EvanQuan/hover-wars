#include "EntityManager.h"
#include "GameManager.h"
#include "CommandHandler.h"
#include "ArtificialIntelligence/AIManager.h"
#include "SceneLoader.h"
#include "ShaderManager.h"
#include "UserInterface/StartInterface.h"
#include "Menus/PostgameMenu.h"
#include "GameStats.h"
#include "UserInterface/GameInterface.h"
#include "Menus/GameMenu.h"
#include "Menus/StartMenu.h"
#include "TextureManager.h"

// Unit: seconds
#define GAME_OVER_TIME 0.0

// Corner Indices
#define BOTTOM_LEFT 0
#define BOTTOM_RIGHT 1
#define TOP_LEFT 2
#define TOP_RIGHT 3

/*************\
 * Constants *
\*************/
const unsigned int FOUR_VEC4 = (sizeof(vec4) << 2);

// Singleton Variable initialization
GameManager* GameManager::m_pInstance = nullptr;

// Constructor - Private, only accessable within the Graphics Manager
GameManager::GameManager(GLFWwindow* rWindow)
{
    // Initialize and Get Shader and Environment Managers
    m_pShaderManager    = SHADER_MANAGER;
    m_pEntityManager    = ENTITY_MANAGER;

    m_pAIManager = AIManager::getInstance();

    // Fetch and update Window HxW
    m_pWindow = rWindow;
    glfwGetWindowSize(m_pWindow, &m_iWidth, &m_iHeight);
    m_pEntityManager->updateWidthAndHeight(m_iWidth, m_iHeight);
    m_pGameStats = nullptr;

    // Initialize Timer Variables
    m_fFrameTime = duration<float>(0.0f);
    m_fMaxDeltaTime = sixtieth_of_a_sec{ 1 };

    m_eKeyboardHovercraft = HOVERCRAFT_PLAYER_1;

    m_pCommandHandler = COMMAND_HANDLER;

    m_pPhysicsManager = PHYSICS_MANAGER;

    // Initialize Render Buffer
    glGenRenderbuffers(1, &m_iRenderBuffer);

    // Generate VAO and VBO for rendering SplitScreen Quads
    glGenVertexArrays(1, &m_iVertexArray);
    m_iVertexBuffer = m_pShaderManager->genVertexBuffer(m_iVertexArray, nullptr, (sizeof(vec4) << 4), GL_STATIC_DRAW);
    m_pShaderManager->setAttrib(m_iVertexArray, 0, 4, sizeof(vec4), nullptr);
}

/*
Singleton Implementations
Requires Window to initialize 

@param rWindow to intialize
*/
GameManager* GameManager::getInstance(GLFWwindow *rWindow)
{
    if (nullptr == m_pInstance)
        m_pInstance = new GameManager(rWindow);

    return m_pInstance;
}

// Does not require window parameter, but assumes that Singleton instance has
// been instantiated beforehand.
GameManager* GameManager::getInstance()
{
    assert(nullptr != m_pInstance);
    return m_pInstance;
}

// Destruct Shaders, Buffers, Arrays and other GL stuff.
GameManager::~GameManager()
{
    // Let go of Window Handle
    m_pWindow = nullptr;

    // Delete Render Buffer
    glDeleteRenderbuffers(1, &m_iRenderBuffer);

    // Split Screen Clean Up
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);

    // Clean up Allocated Memory
    // NOTE: crash at glDeleteBuffers in destructor
    if (nullptr != m_pEntityManager)    // Entity Manager
        delete m_pEntityManager;

    if (nullptr != m_pShaderManager)    // Shader Manager
        delete m_pShaderManager;

    // User Interface
    for (UserInterface* ui : m_vInterfaceInstances) {
        delete ui;
    }
    m_vInterfaceInstances.clear();

    if (nullptr != m_pCommandHandler)   // Command Handler
        delete m_pCommandHandler;

    if (nullptr != m_pAIManager)        // AI Manager
        delete m_pAIManager;

    if (nullptr != m_pPhysicsManager)
        delete m_pPhysicsManager;

    if (nullptr != m_pGameStats)        // Game Stats
        delete m_pGameStats;
}


/*
    As UserInterface instances are generated, they are each added to
    the m_vInterfaceInstances list.
    At the end of the program, all interfaces are deleted.
*/
void GameManager::addInterface(UserInterface* ui)
{
    m_vInterfaceInstances.push_back(ui);
}
void GameManager::setCurrentInterface(UserInterface* ui)
{
    m_pCurrentInterface = ui;
}
/*
    Start rendering game to screen. This call with block until the game loop
    ends (it will hang the thread). When this function returns, the program
    should end.
*/
void GameManager::startRendering()
{
    SOUND_MANAGER->start();
    resetTime();

    while (renderGraphics());
}
/*
    Render the graphics of a single frame to the screen.
    Intended to be called every cycle, or when the graphics need to be updated

    @return true if the game should continue to run. In other words,
            renderGraphics() should continue to be called within the rendering
            loop.
*/
bool GameManager::renderGraphics()
{
    // Update Timer
    m_pTimer.updateTimeSinceLastFrame();
    std::chrono::duration<double> fSecondsSinceLastFrame = m_pTimer.getFrameTimeSinceLastFrame();
    m_fFrameTime += fSecondsSinceLastFrame;
    /*
    Get the delta since the last frame and update based on that delta.
    Do not confuse this with EntityManager's delta time, which is much smaller
    since it updates more frequently than every frame update.

    Unit: seconds
    */
    float frameDeltaTime = static_cast<float>(fSecondsSinceLastFrame.count());
    // Execute all commands for this frame
    // These should be done before the EntityManager updates so that the
    // environemnt can respond to the commands issued this frame.
    m_pCommandHandler->update(frameDeltaTime);
    m_fGameTime -= frameDeltaTime;
    if (!startedGameOver && (m_fGameTime < 0))
    {
        startedGameOver = true;
    }

    // Update Environment if the game is not paused
    if (!m_bPaused)
    {
        m_pAIManager->update(frameDeltaTime);
        m_pEntityManager->updateEnvironment(fSecondsSinceLastFrame);
        if (startedGameOver)
        {
            // Decrease real time
            m_fGameOverTime -= frameDeltaTime;
            if (m_fGameOverTime <= 0)
            {
                endGame();
            }
            else
            {
                // Do whatever you want for this duration.
                // Slow mo?
                // Music change/fade?
            }
        }

        // The user interface should update after the EntityManager and
        // CommandHandler has changed in order to reflect their changes.
        // It also cannot update inside the EntityManager since it is able
        // to be updated while the EntityManager is paused.
        m_pCurrentInterface->update(frameDeltaTime);
        // call function to draw our scene
        drawScene();
    }

    // Sound needs to update after the EntityManager to reflect in game changes
    // Cannot be updated inside the EntityManager as sounds can play while game
    // is paused.
    SOUND_MANAGER->update();


    // check for Window events
    glfwPollEvents();

    return !glfwWindowShouldClose(m_pWindow);
}

/*
    Initialize everything necessary to start a new game.

    @param playerCount  player hovercrafts to register
    @param botCount     bot hovercrafts to register
    @param gameTime     of game, in seconds
    @param sFileName    of .scene environment to load
*/
void GameManager::initializeNewGame(unsigned int playerCount,
                                    unsigned int botCount,
                                    float gameTime,
                                    string sFileName)
{
    // We intialize all values for the game to immediately start
    // Initialize Physics
    m_pPhysicsManager = PHYSICS_MANAGER;
    m_pPhysicsManager->initPhysics(true);
    // m_pPhysicsManager->cleanupPhysics();
    // delete m_pPhysicsManager;
    // m_pPhysicsManager = PHYSICS_MANAGER;
    // m_pPhysicsManager->initPhysics(false);

    m_bPaused = false;
    startedGameOver = false;
    m_fGameTime = gameTime;
    m_fGameOverTime = GAME_OVER_TIME;
    TextureManager* pTxtMngr = TEXTURE_MANAGER;
    GameInterface::getInstance(m_iWidth, m_iHeight)->reinitialize(gameTime);
    m_pEntityManager->initializeEnvironment(sFileName);

    // Figure out Frame Buffer Sizes for multiple players
    m_iSplitHeight = m_iHeight;
    m_iSplitWidth = m_iWidth;
    if (playerCount > 1)
        m_iSplitHeight <<= 1;
    if (playerCount > 2)
        m_iSplitWidth <<= 1;

    // Set up Render Buffer for all the Frames
    glBindRenderbuffer(GL_RENDERBUFFER, m_iRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iSplitWidth, m_iSplitHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Spawn Players
    for (unsigned int i = 0; i < playerCount; i++) {
        SCENE_LOADER->createPlayer();
        generateFrameBuffer(i);
    }

    // Spawn Bots
    for (unsigned int i = 0; i < botCount; i++) {
        SCENE_LOADER->createBot();
    }

    // AFTER the players and bots have been made, the GameStats and AI
    // need to reinitialize to track the players and bots
    m_pGameStats->reinitialize(playerCount, botCount);
    m_pAIManager->reinitialize();
}

// Name: generateFrameBuffer
// 
void GameManager::generateFrameBuffer(unsigned int iPlayer)
{
    // Local Variables
    float fHeight, fWidth, fSplitHeight, fSplitWidth;

    // Get Dimensions in Floating point values
    fHeight = static_cast<float>(m_iHeight);
    fWidth = static_cast<float>(m_iWidth);
    fSplitHeight = fHeight * 0.5f;
    fSplitWidth = fWidth * 0.5f;

    vec4 vCorners[4] = {
        vec4(0.0f,    0.0f,     0.0f, 1.0f), /*Bottom Left*/
        vec4(fWidth,  0.0f,     1.0f, 1.0f), /*Bottom Right*/
        vec4(0.0f,    fHeight,  0.0f, 0.0f), /*Top Left*/
        vec4(fWidth,  fHeight,  1.0f, 0.0f), /*Top Right*/
    };

    /*
        (0, 1)-----(0.5, 1)------(1, 1)
          |            |           |
        (0, 0.5)---(0.5, 0.5)----(1, 0.5)
          |            |           |
        (0, 0)-----(0.5, 0)------(1, 0)
    */
    // Modify Screen Space Data
    switch (iPlayer)
    {
    case 0:
        if (m_iSplitHeight != m_iHeight)    // > 1 Player? -> compact the frame to half height
            vCorners[BOTTOM_LEFT].y = vCorners[BOTTOM_RIGHT].y = fSplitHeight;
        if (m_iSplitWidth != m_iWidth)      // > 2 Players? -> compact the frame to half width
            vCorners[BOTTOM_RIGHT].x = vCorners[TOP_RIGHT].x = fSplitWidth;
        break;
    case 1:
        if (m_iSplitWidth != m_iWidth)      // > 2 Players? compact frame to top right quadrant
        {
            vCorners[BOTTOM_LEFT].y = vCorners[BOTTOM_RIGHT].y = fSplitHeight;
            vCorners[BOTTOM_LEFT].x = vCorners[TOP_LEFT].x = fSplitWidth;
        }
        else                                // exactly 2 players? compact frame to bottom half
            vCorners[TOP_LEFT].y = vCorners[TOP_RIGHT].y = fSplitHeight;
        break;
    case 2:                                 // Compact to bottom left quadrant
        vCorners[TOP_LEFT].y = vCorners[TOP_RIGHT].y = fSplitHeight;
        vCorners[TOP_RIGHT].x = vCorners[BOTTOM_RIGHT].x = fSplitWidth;
        break;
    case 3:                                 // Compact to bottom right quadrant
        vCorners[TOP_LEFT].y = vCorners[TOP_RIGHT].y = fSplitHeight;
        vCorners[TOP_LEFT].x = vCorners[BOTTOM_LEFT].x = fSplitWidth;
        break;
    }

    // Store data in VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, iPlayer * FOUR_VEC4, FOUR_VEC4, vCorners);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_pFrameBuffers.push_back(TEXTURE_MANAGER->genFrameBuffer(m_iSplitWidth, m_iSplitHeight));
}

/*
    End the current game.

    - Stop the AI
    - Stop Physics
    - Determine the winner
    - Move to the PostGameMenu
*/
void GameManager::endGame()
{
    SOUND_MANAGER->play(SoundManager::eSoundEvent::SOUND_UI_END_GAME_CHEER);

    // postgame menu
    cout << "GameManger::endGame()" << endl;
    m_bPaused = true;
    COMMAND_HANDLER->setCurrentMenu(PostgameMenu::getInstance());
    m_pEntityManager->purgeEnvironment();
    TextureManager* pTxtMngr = TEXTURE_MANAGER;

    // Clean up Frame Buffers
    for each (Texture* tFrameBuffer in m_pFrameBuffers)
        pTxtMngr->unloadTexture(&tFrameBuffer);
    m_pFrameBuffers.clear();

    resizeWindow(m_iWidth, m_iHeight);

    // m_pPhysicsManager->cleanupPhysics();
    if (nullptr != m_pPhysicsManager)
    {
        delete m_pPhysicsManager;
        m_pPhysicsManager = nullptr;
    }
}

/*
    Draw the scene with a 60 fps lock.
    In other words, if drawScene() is called over 60 times per second, the
    scene will not be drawn more than it needs to.
*/
void GameManager::drawScene()
{
    if (m_fFrameTime >= sixtieth_of_a_sec{ 1 }) // This locks the framerate to 60 fps
    {
        m_fFrameTime = seconds{ 0 };

        // Render the Scene
        glEnable(GL_DEPTH_TEST);
        if (!m_bPaused)
        {
            // Set up Render for this frame
            m_pEntityManager->setupRender();

            // Render each screen
            for( unsigned int i = 0; i < m_pFrameBuffers.size(); ++i)
            {
                // Bind Frame Buffer
                glBindFramebuffer(GL_FRAMEBUFFER, m_pFrameBuffers[i]->getTextureID());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_iRenderBuffer);
                assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

                // Render Frame
                m_pEntityManager->renderEnvironment(i);
            }

            // Render the Split Screen Quads
            renderSplitScreen();
        }
        m_pCurrentInterface->render();
        glDisable(GL_DEPTH_TEST);

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(m_pWindow);
    }
}

// Name: renderSplitScreen
// Written by: James Coté
// Description: Draws each Frame Buffer into set up quads in screen space.
//              >2 Player Layout:
//                  Player 1        Player 2
//                  Player 3        Player 4
//              2 Player Layout:
//                  Player 1
//                  Player 2
void GameManager::renderSplitScreen()
{
    // Set up OpenGL for Rendering
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShaderManager->getProgram(ShaderManager::eShaderType::UI_SHDR));
    m_pShaderManager->setUniformBool(ShaderManager::eShaderType::UI_SHDR, "isImage", true);

    // Bind each Frame Buffer and Render image
    for (unsigned int i = 0; i < m_pFrameBuffers.size(); ++i)
    {
        m_pFrameBuffers[i]->bindTexture(ShaderManager::eShaderType::UI_SHDR, "text");
        glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

/*
    Initialize all start-of-program state.
    This should only be called once at the start of the game.

    Shaders:
        Initialize shaders and geometry.
        Contains any initializion requirements in order to start drawing.

    Debug:
        Initialize a new game and immediately enter the game menu and interface

    Release:
        Set the game to use the start menu and and start interface

    @return true if initialzation successful
*/
bool GameManager::initialize()
{
    // Shaders
    if (!m_pShaderManager->initializeShaders())
    {
        cout << "Couldn't initialize shaders." << endl;
        // Note: in release mode, there is some error in initializing the shaders.
        // For now, we will continue loading the rest of the game as normal.
        return false;
    }

    // Initialize Environment with a new scene      
    // For now we will initialize this here instead of the constructor as GameStats
    // uses GameInterface, which requires GameManager to already be instantiated.
    m_pGameStats     = GameStats::getInstance(m_iWidth, m_iHeight);

#ifdef NDEBUG
    // Game starts paused as the player starts in the start menu
    m_bPaused = true;
    startedGameOver = false;
    m_fGameOverTime = GAME_OVER_TIME;
    m_pCommandHandler->setCurrentMenu(StartMenu::getInstance());
    m_pCurrentInterface = StartInterface::getInstance(m_iWidth, m_iHeight);
#else
    initializeNewGame(1, 0, 9999999.0f, RELEASE_ENV);
    // initializeNewGame(1, 4, 9999999.0f, DEBUG_ENV);
    m_pCommandHandler->setCurrentMenu(GameMenu::getInstance()); 
    m_pCurrentInterface = GameInterface::getInstance(m_iWidth, m_iHeight);
#endif

    // Return error results
    return true; 
}

/*******************************************************************************\
 * Camera Manipulation                                                         *
\*******************************************************************************/

void GameManager::rotateCamera(vec2 pDelta)
{
    m_pEntityManager->rotateCamera(pDelta);
}

void GameManager::zoomCamera(float fDelta)
{
    m_pEntityManager->zoomCamera(fDelta);
}

/*
Called from Window Resize callback.
Currently updates the Entity Manager with new Window Size, may require more
functionality for menus, etc.

@param iWidth   of the window after resizing
@param iHeight  of the window after resizing
*/
void GameManager::resizeWindow( int iWidth, int iHeight )
{
    // Store Screen Size
    m_iHeight = m_iSplitHeight = iHeight;
    m_iWidth = m_iSplitWidth = iWidth;
    unsigned int iSize = m_pFrameBuffers.size();

    // Calculate SplitScreen Size
    if (iSize > 1)
        m_iSplitHeight <<= 1;
    if (iSize > 2)
        m_iSplitWidth <<= 1;

    m_pEntityManager->updateWidthAndHeight(m_iSplitWidth, m_iSplitHeight);
    m_pCurrentInterface->updateWidthAndHeight(iWidth, iHeight);
}

/*
Calculates an intersection given screen coordinates.
This is used for testing the particle emitter by spawning an emitter where
the mouse clicks the floor plane.

@param fX   x-coordinate of the mouse in window coordinates
@param fY   y-coordinate of the mouse in window coordinates
*/
void GameManager::intersectPlane(float fX, float fY)
{
    // Local Variables
    vec3 vRay = m_pEntityManager->getActiveCameraComponent()->getRay(fX, fY);
    vec3 vNormal = vec3(0.0, 1.0, 0.0); // normal of xz-plane
    vec3 vCameraPos = m_pEntityManager->getActiveCameraComponent()->getCameraWorldPos();
    vec3 vIntersection = vec3(-1.0f);
    float fT = dot(vRay, vNormal);

    // Calculate Intersection
    if ((fT > FLT_EPSILON) || (fT < -FLT_EPSILON))
    {
        // Is intersecting.
        fT = -(dot(vCameraPos, vNormal) / fT);

        // Not behind camera.
        if (fT >= 0)
            vIntersection = vCameraPos + (fT*vRay);

        EMITTER_ENGINE->generateEmitter(vIntersection, vNormal, 60.f, 5.0f, 100, false, 2.0f);
    }
}
