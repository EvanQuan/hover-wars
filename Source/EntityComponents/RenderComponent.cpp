#include "EntityComponentHeaders/RenderComponent.h"

// Default Constructor:
//		Requires an EntityID for the Entity that the component is a part of
//			and a ComponentID issued by the EntityManager.
RenderComponent::RenderComponent(int iEntityID, int iComponentID, bool bStaticDraw, 
								 ShaderManager::eShaderType eType, GLenum eMode )
	: EntityComponent( iEntityID, iComponentID )
{
	m_bStaticDraw = bStaticDraw;
	m_bUsingIndices = false;
	m_bUsingInstanced = false;
	m_eShaderType = eType;
	m_eMode = eMode;
	glGenVertexArrays(1, &m_iVertexArray);
	m_pShdrMngr = SHADER_MANAGER;
}

// Destructor
RenderComponent::~RenderComponent()
{
	glDeleteBuffers(1, &m_iVertexBuffer);
	glDeleteVertexArrays(1, &m_iVertexArray);
}

// Loads the GPU and calls openGL to render.
void RenderComponent::render()
{
	// Set up OpenGL state
	glBindVertexArray(m_iVertexArray);
	glUseProgram(m_pShdrMngr->getProgram(m_eShaderType));

	// Bind Texture(s) HERE

	// Call related glDraw function.
	if (m_bUsingInstanced)
		glDrawElementsInstanced(m_eMode, 0, GL_UNSIGNED_INT, 0, 0);
	else if (m_bUsingIndices)
		glDrawElements(m_eMode, 0, GL_UNSIGNED_INT, nullptr);
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

void RenderComponent::initializeComponent(const vector<vec3>& vVertices, const vector<vec3>& vNormals, const vector<vec2>& vUVs)
{
	// This function shouldn't be called without passing in Vertex Data.
	assert(!vVertices.empty());

	// For Static objects, we can use just one VBO since it won't be updated as often.
	//	Therefore, we'll combine all data into one VBO and set openGL to address the data accordingly.
	if (m_bStaticDraw)
	{
		// Create a Vector to hold the combined data.
		vector<float> vVNdata;
		vVNdata.reserve(vVertices.size() * 3 +
						vNormals.size() * 3 + 
						(vUVs.size() << 1) );

		// Boolean values to determine if additional data exists.
		bool bHaveNormals = !vNormals.empty();
		bool bHaveUVs = !vUVs.empty();

		// Calculate Stride for setting up Attributes
		GLsizei iStride = sizeof(vec3) + 
						  (bHaveNormals ? sizeof(vec3) : 0 ) +
						  (bHaveUVs ? sizeof(vec2) : 0 );

		// Loop through the received input and set up the array of data for the VBO
		for (unsigned int i = 0; i < vVertices.size(); ++i)
		{
			// Vertex Data
			vVNdata.push_back(vVertices[i].x);
			vVNdata.push_back(vVertices[i].y);
			vVNdata.push_back(vVertices[i].z);

			// Normal Data
			if (bHaveNormals)
			{
				vVNdata.push_back(vNormals[i].x);
				vVNdata.push_back(vNormals[i].y);
				vVNdata.push_back(vNormals[i].z);
			}

			// UV Data
			if (bHaveUVs)
			{
				vVNdata.push_back(vUVs[i].x);
				vVNdata.push_back(vUVs[i].y);
			}
		}

		// Generate VBO
		m_iVertexBuffer = m_pShdrMngr->genVertexBuffer(m_iVertexArray, vVNdata.data(), vVNdata.size() * sizeof(float), GL_STATIC_DRAW);

		// Set-up Attributes
		// Vertices
		m_pShdrMngr->setAttrib(m_iVertexArray, 0, 3, iStride, (void*)0);
		// Normals
		if (bHaveNormals)
			m_pShdrMngr->setAttrib(m_iVertexArray, 1, 3, iStride, (void*)sizeof(vec3));
		// UVs
		if (bHaveUVs) // Specified index could be 1 or 2 and Start location is Stride - sizeof(vec2) depending on if Normals exist. 
			m_pShdrMngr->setAttrib(m_iVertexArray, 2, 2, iStride, (void*)(iStride - sizeof(vec2)));

		// Set up Count for Drawing Elements
		m_iCount = vVertices.size();
	}
}