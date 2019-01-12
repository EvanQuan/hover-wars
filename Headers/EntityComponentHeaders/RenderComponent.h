#pragma once
#include "stdafx.h"
#include "EntityComponent.h"
#include "ShaderManager.h"

/************************************************************
* Name: RenderComponent
* Written by: James Cote
* Description: Component used for storing vertex information and handles
*	for the GPU to render to the screen.
***************************************************************/

class RenderComponent 
	: public EntityComponent
{
public:
	RenderComponent(int iEntityID, int iComponentID, bool bStaticDraw, 
					ShaderManager::eShaderType eType, GLenum eMode);
	virtual ~RenderComponent();

	// render will call the GPU to draw the bound data.
	void render();

	// Inherited update frunction from EntityComponent
	void update(double dTimeStep);

	// Initializes the proper buffers on the GPU for rendering.
	void initializeComponent(const vector<vec3>& vVertices, const vector<vec3>& vNormals, const vector<vec2>& vUVs);

private: 
	// Private Copy Constructor and Assignment operator overload.
	RenderComponent(const RenderComponent* pCopy);
	RenderComponent& operator=(const RenderComponent& pRHS);

	// Private Variables
	GLuint m_iVertexArray, m_iVertexBuffer, m_iNormalBuffer, m_iTextureBuffer;
	GLuint m_iInstancedBuffer, m_iIndicesBuffer;
	GLuint m_iNumInstances;
	GLenum m_eMode;
	GLsizei m_iCount;
	bool m_bStaticDraw, m_bUsingIndices, m_bUsingInstanced;
	ShaderManager* m_pShdrMngr;
	ShaderManager::eShaderType m_eShaderType;
};
