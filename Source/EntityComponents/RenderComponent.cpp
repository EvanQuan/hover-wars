#include "EntityComponentHeaders/RenderComponent.h"

// Default Constructor:
//		Requires an EntityID for the Entity that the component is a part of
//			and a ComponentID issued by the EntityManager.
RenderComponent::RenderComponent(int iEntityID, int iComponentID, bool bStaticDraw, 
								 ShaderManager::eShaderType eType, GLenum eMode )
	: EntityComponent( iEntityID, iComponentID )
{
	m_bUsingIndices = false;
	m_bUsingInstanced = false;
	m_eShaderType = eType;
	m_eMode = eMode;
	m_pShdrMngr = SHADER_MANAGER;
}

// Destructor
RenderComponent::~RenderComponent()
{

}

// Loads the GPU and calls openGL to render.
void RenderComponent::render()
{
	// Set up OpenGL state
	glBindVertexArray(m_pMesh->getVertexArray());
	glUseProgram(m_pShdrMngr->getProgram(m_eShaderType));

	// Bind Texture(s) HERE

	// Call related glDraw function.
	if (m_bUsingInstanced)
		glDrawElementsInstanced(m_eMode, 0, GL_UNSIGNED_INT, 0, 0);
	else if (m_bUsingIndices)
		glDrawElements(m_eMode, m_iCount, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(m_eMode, 0, m_iCount);

	// Unbind Texture(s) HERE

	// Clean up OpenGL state
	glUseProgram(0);
	glBindVertexArray(0);
}

// Overloaded Update Function
void RenderComponent::update(double dTimeDelta)
{

}

void RenderComponent::initializeComponent(const Mesh* pMesh, const string* pTextureLoc)
{
	// Get number of Vertices.
	m_iCount = pMesh->getCount();

	if (nullptr != pTextureLoc)
	{
		
	}

	m_bUsingIndices = pMesh->usingIndices();

	// Store Mesh for Reference.
	m_pMesh = pMesh;
}