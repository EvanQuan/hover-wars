#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "Camera.h"
#include <time.h>

// Forward Declarations
class EntityManager;
class InputHandler;
class ShaderManager;

// Class: Graphics Manager
// Purpose: Acts as the Sinew between all moving parts that are required for drawing
//			with openGL.
// TODO: Set-up a Manager for Geometry (Geometry may be expanded on later).
// Written by: James Cot�
class GameManager
{
public:
	static GameManager* getInstance(GLFWwindow *rWindow);
	~GameManager();
	
	// Graphics Application
	bool initializeGraphics( string sFileName );
	bool renderGraphics();

	/// HxW Settings
	void resizedWindow(int iHeight, int iWidth);

	// Camera Functions 
	//void snapShotCamera();
	void rotateCamera(vec2 pDelta);
	void zoomCamera(float fDelta);
	void moveCamera( vec3 pDirection );
	void switchView();

	// Helper Enum for RGB Values
	enum eRGB
	{
		RED = 0,
		GREEN,
		BLUE,
		RGB_MAX
	};

	// Must be set after getInstance() to avoid mutual getInstance() recursion
	// with InputHandler/CommandHandler
	InputHandler* m_inputHandler;

private:
	// For Singleton Implementation
	GameManager(GLFWwindow* rWindow); 
	GameManager(const GameManager* pCopy);
	static GameManager* m_pInstance;

	// Window Reference
	GLFWwindow* m_pWindow;
	Time m_pTimer;

	// Axis Buffer/Array Containers
	GLuint m_pVertexArray;
	GLuint m_pVertexBuffer;

	// Camera
	Camera* m_pCamera;
	enum cView
	{
		VIEW_SPHERICAL = 0,
		VIEW_FPS,
		VIEW_FOLLOW,
		VIEW_MAX
	} m_eView;

	// Render Functions
	void RenderScene();
	void renderAxis();

	// Manages Shaders for all assignments
	EntityManager* m_pEntMngr;
	ShaderManager* m_pShaderMngr;

};
