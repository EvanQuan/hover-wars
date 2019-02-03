#include "EntityManager.h"
#include "GameManager.h"
#include "CommandHandler.h"
#include "Scene_Loader.h"
#include "ShaderManager.h"
#include "UserInterface.h"

///////////////
// CONSTANTS //
///////////////
const vec3 WORLD_CENTER = vec3( 0.0 );
const mat3 WORLD_COORDS = mat3( 1.0 );
const vector<vec3> AXIS_VERTS = { WORLD_CENTER, WORLD_COORDS[ 0 ],
								  WORLD_CENTER, WORLD_COORDS[ 1 ],
								  WORLD_CENTER, WORLD_COORDS[ 2 ] };

// Singleton Variable initialization
GameManager* GameManager::m_pInstance = nullptr;

// Constructor - Private, only accessable within the Graphics Manager
GameManager::GameManager(GLFWwindow* rWindow)
{
	// Initialize and Get Shader and Environment Managers
	m_pShaderMngr    = SHADER_MANAGER;
	m_pEntMngr       = ENTITY_MANAGER;
	m_pUserInterface = USER_INTERFACE;

	// NOTE: Do not get an instance of InputHandler here or there will be
	// infinite mutual recursion and a call stack overflow

	m_pWindow = rWindow;
	int iHeight, iWidth;
	glfwGetWindowSize(m_pWindow, &iWidth, &iHeight);

	glGenVertexArrays( 1, &m_pVertexArray );
	
	// Generate Buffer and Set Attribute
	m_pVertexBuffer = m_pShaderMngr->genVertexBuffer( m_pVertexArray, AXIS_VERTS.data(), AXIS_VERTS.size() * sizeof( vec3 ), GL_STATIC_DRAW );
	SHADER_MANAGER->setAttrib(m_pVertexArray, 0, 3, 0, nullptr);
}

// Singleton Implementations
// Requires Window to initialize 
GameManager* GameManager::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new GameManager(rWindow);
	}

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

	// Let go of Manager Handles
	if (nullptr != m_pEntMngr)
	{
		delete m_pEntMngr;
	}

	if (nullptr != m_pShaderMngr)
	{
		delete m_pShaderMngr;
	}

	if (nullptr != m_pUserInterface)
	{
		delete m_pUserInterface;
	}

	glDeleteBuffers(1, &m_pVertexBuffer);
	glDeleteVertexArrays(1, &m_pVertexArray);
}

// Intended to be called every cycle, or when the graphics need to be updated
bool GameManager::renderGraphics()
{
	// Update Timer
	m_pTimer.updateTime();

	// Execute all commands for this frame
	m_commandHandler->executeAllCommands();

	// Update Environment
	m_pEntMngr->updateEnvironment(m_pTimer);

	// call function to draw our scene
	RenderScene();

	// scene is rendered to the back buffer, so swap to front for display
	glfwSwapBuffers(m_pWindow);

	// check for Window events
	glfwPollEvents();

	return !glfwWindowShouldClose(m_pWindow);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer
// Copied from Boilercode Program
// Will be replaced with functions in Graphic objects.
void GameManager::RenderScene()
{
	// HACK: For following Car on Roller Coaster Assignment, Remove
	//mat4 pFreNetFrame = (VIEW_SPHERICAL == m_eView) ? mat4( 1.0 ) : m_pEntMngr->getFrenetFrame();
	const CameraComponent* pCamera = m_pEntMngr->getActiveCamera();

	mat4 pModelViewMatrix = pCamera->getToCameraMat();
	mat4 pProjectionMatrix = pCamera->getPerspectiveMat();
	vec3 vCamLookAt = pCamera->getLookAt();

	GLfloat color[] = { 0.3215f, 0.3411f, 0.4352f, 1.0f };
	const GLfloat zero = 1.0f;

	glClearBufferfv(GL_COLOR, 0, color);
	glClearBufferfv(GL_DEPTH, 0, &zero);
	glEnable(GL_DEPTH_TEST);
	
	// Set camera information in Shaders before rendering
	m_pShaderMngr->setProjectionModelViewMatrix( &pProjectionMatrix, &pModelViewMatrix );

	//renderAxis();
	m_pEntMngr->renderEnvironment( vCamLookAt );
	glDisable(GL_DEPTH_TEST);
}

void GameManager::renderAxis()
{
	glPointSize( 10.f );
	CheckGLErrors();

	glBindVertexArray( m_pVertexArray );
	glUseProgram( m_pShaderMngr->getProgram( ShaderManager::eShaderType::WORLD_SHDR ) );

	glDrawArrays( GL_LINES, 0, AXIS_VERTS.size() );
	glDrawArrays( GL_POINTS, 0, AXIS_VERTS.size() );

	glUseProgram( 0 );
	glBindVertexArray( 0 );

	glPointSize( 1.f );
}

// Function initializes shaders and geometry.
// contains any initializion requirements in order to start drawing.
bool GameManager::initializeGraphics( string sFileName )
{
	// Locals
	bool bError = false;

	// Shaders
	if (!m_pShaderMngr->initializeShaders())
	{
		cout
			<< "Couldn't initialize shaders." << endl;
		bError = true;
	}
	else
		m_pEntMngr->initializeEnvironment(sFileName);

	// Set up Camera
	m_pCamera = m_pEntMngr->generateCameraEntity();
	m_eView = VIEW_SPHERICAL;

	return bError; 
}

/*******************************************************************************\
 * Camera Manipulation                                                         *
\*******************************************************************************/

void GameManager::rotateCamera(vec2 pDelta)
{
	m_pCamera->orbit(pDelta);
}

void GameManager::zoomCamera(float fDelta)
{
	m_pCamera->zoom(fDelta);
}

void GameManager::switchView()
{
	//m_eView = (cView) (m_eView + 1);
	//m_eView = m_eView >= VIEW_MAX ? VIEW_SPHERICAL : m_eView;
	//
	//switch ( m_eView )
	//{
	//	default:
	//	case VIEW_SPHERICAL:
	//		m_pCamera->setLookAt( vec3( 0.0 ) );
	//	case VIEW_FOLLOW:
	//		m_pCamera->positionCamera( mat4( 1.0 ) );
	//		m_pCamera->setSteady( false );
	//		break;
	//	case VIEW_FPS:
	//		m_pCamera->setSteady( true );
	//		break;
	//}
}

void GameManager::resizedWindow( int iHeight, int iWidth )
{
	m_pEntMngr->updateHxW(iHeight, iWidth);
}
