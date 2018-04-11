#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "Camera.h"

// Forward Declarations
class ShaderManager;
class EnvironmentManager;

// Class: Graphics Manager
// Purpose: Acts as the Sinew between all moving parts that are required for drawing
//			with openGL.
// TODO: Set-up a Manager for Geometry (Geometry may be expanded on later).
// Written by: James Coté
class GraphicsManager
{
public:
	static GraphicsManager* getInstance(GLFWwindow *rWindow);
	~GraphicsManager();
	
	// Graphics Application
	bool initializeGraphics( string sFileName );
	bool renderGraphics();

	/// HxW Settings
	void resizedWindow( int iHeight, int iWidth ) { m_pCamera->updateHxW( iHeight, iWidth ); }

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

	// Shader Manipulation functions
	void setRGBVal( eRGB eType, float fVal);
	void setBeta(float fVal);
	void setAlpha(float fVal);
	void setBVal(float fVal);
	void setYVal(float fVal);
	void togGooch();
	void togToon();
	void togSpec();
	void setShine( float fVal );
	void setR( float fVal );

private:
	// For Singleton Implementation
	GraphicsManager(GLFWwindow* rWindow); 
	GraphicsManager(const GraphicsManager* pCopy);
	static GraphicsManager* m_pInstance;

	// Window Reference
	GLFWwindow* m_pWindow;

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
	ShaderManager* m_pShaderMngr;
	EnvironmentManager* m_pEnvMngr;
};

