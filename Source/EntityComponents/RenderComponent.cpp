#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityManager.h"

const string DEFAULT_DIFFUSE_MAP = "textures/defaultTexture.jpg";
const string DEFAULT_SPECULAR_MAP = "textures/defaultSpecMap.jpg";

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
	m_pShdrMngr->setUniformFloat(m_eShaderType, "fScale", m_pMesh->getScale());

	// Bind Texture(s) HERE
 	m_sRenderMaterial.m_pDiffuseMap->bindTexture(m_eShaderType, "sMaterial.vDiffuse");
	m_sRenderMaterial.m_pSpecularMap->bindTexture(m_eShaderType, "sMaterial.vSpecular");
	m_pShdrMngr->setUniformFloat(m_eShaderType, "sMaterial.fShininess", m_sRenderMaterial.fShininess);

	// Call related glDraw function.
	if (m_bUsingInstanced)
		glDrawElementsInstanced(m_eMode, m_pMesh->getCount(), GL_UNSIGNED_INT, 0, m_pMesh->getNumInstances());
	else if (m_bUsingIndices)
		glDrawElements(m_eMode, m_iCount, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(m_eMode, 0, m_iCount);

	m_pMesh->updateEdgeBuffer(ENTITY_MANAGER->getActiveCamera()->getLookAt());
	m_pMesh->renderEdgeBuffer( 90.f, 180.f );

	// Unbind Texture(s) HERE
	m_sRenderMaterial.m_pDiffuseMap->unbindTexture();
	m_sRenderMaterial.m_pSpecularMap->unbindTexture();
}

// Overloaded Update Function
void RenderComponent::update(double dTimeDelta)
{

}

void RenderComponent::initializeComponent(const Mesh* pMesh, 
										  const string* pDiffuseTextureLoc,
										  const string* pSpecularTextureLoc)
{
	// Get number of Vertices.
	m_iCount = pMesh->getCount();

	// Load Diffuse Texture if applicable
	if (nullptr != pDiffuseTextureLoc)
		m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture((*pDiffuseTextureLoc));

	// If Texture failed to load, load default
	if( nullptr == m_sRenderMaterial.m_pDiffuseMap)
		m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(DEFAULT_DIFFUSE_MAP);

	// Load Texture if applicable
	if (nullptr != pSpecularTextureLoc)
		m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->loadTexture((*pSpecularTextureLoc));
	
	// If Texture failed to load, load default
	if (nullptr == m_sRenderMaterial.m_pSpecularMap)
		m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->loadTexture(DEFAULT_SPECULAR_MAP);


	// Check Rendering Flags in Mesh.
	m_bUsingIndices = pMesh->usingIndices();
	m_bUsingInstanced = pMesh->usingInstanced();

	// Store Mesh for Reference.
	m_pMesh = pMesh;
}

// Generates a simple 1x1 diffuse texture based on the given color.
void RenderComponent::generateDiffuseTexture(const vec3* vColor)
{
	m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->genTexture(vColor);
}
