#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityManager.h"

const string DEFAULT_DIFFUSE_MAP = "textures/defaultTexture.jpg";
const vec4 DEFAULT_SPEC_COLOR = vec4(vec3(0.f), 1.0f);

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
	m_sRenderMaterial.fShininess = 0.0f;
	m_sRenderMaterial.m_pDiffuseMap = nullptr;
	m_sRenderMaterial.m_pSpecularMap = nullptr;
}

// Destructor
RenderComponent::~RenderComponent()
{

}

// Loads the GPU and calls openGL to render.
void RenderComponent::render()
{
	// Set up OpenGL state
	glBindVertexArray(m_iVertexArray);
	glUseProgram(m_pShdrMngr->getProgram(m_eShaderType));

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

	// Unbind Texture(s) HERE
	m_sRenderMaterial.m_pDiffuseMap->unbindTexture();
	m_sRenderMaterial.m_pSpecularMap->unbindTexture();
}

// Overloaded Update Function
void RenderComponent::update(double dTimeDelta)
{

}

void RenderComponent::initializeComponent(const Mesh* pMesh, 
										  const Material* pMaterial)
{
	// Ensure the Mesh passed in is valid.
	assert(nullptr != pMesh);

	// Get number of Vertices.
	m_iCount = pMesh->getCount();

	// Load the Material
	loadMaterial(pMaterial);

	// Check Rendering Flags in Mesh.
	m_bUsingIndices = pMesh->usingIndices();
	m_bUsingInstanced = pMesh->usingInstanced();

	// Store Mesh for Reference.
	m_pMesh = pMesh;
	m_iVertexArray = pMesh->getVertexArray();	// Store Vertex Array Locally.
}

// Generates a simple 1x1 diffuse texture based on the given color.
void RenderComponent::generateDiffuseTexture(const vec4* vColor)
{
	m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->genTexture(vColor);
}

// Function to load a Material. Checks passed in material and loads a default if necessary.
// Overwrites any currently loaded Material with the new material if applicable. 
//	Texture Manager manages the textures using unique pointers, therefore, no memory is lost by simply replacing existing textures.
void RenderComponent::loadMaterial(const Material* pMaterial)
{
	if (nullptr != pMaterial)
	{
		// Load Diffuse Texture if applicable
		if ("" != pMaterial->sDiffuseMap)
			m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(pMaterial->sDiffuseMap);

		// Load Texture if applicable
		if ("" != pMaterial->sOptionalSpecMap)
			m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->loadTexture(pMaterial->sOptionalSpecMap);
		else	// "" as Spec Map Location? just generate a texture from whatever the Spec Shade is.
			m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->genTexture(&pMaterial->vOptionalSpecShade);

		// Store Shininess
		m_sRenderMaterial.fShininess = pMaterial->fShininess;
	}

	// Set some defaults if no Maps were specified.
	if (nullptr == m_sRenderMaterial.m_pDiffuseMap)
		m_sRenderMaterial.m_pDiffuseMap = TEXTURE_MANAGER->loadTexture(DEFAULT_DIFFUSE_MAP);
	if (nullptr == m_sRenderMaterial.m_pSpecularMap)
		m_sRenderMaterial.m_pSpecularMap = TEXTURE_MANAGER->genTexture(&DEFAULT_SPEC_COLOR);
}

void RenderComponent::getDiffuseTextureDimensions(int* iHeight, int* iWidth)
{
	if (nullptr != m_sRenderMaterial.m_pDiffuseMap)
		m_sRenderMaterial.m_pDiffuseMap->getTextureDimensions(iHeight, iWidth);
}