#include "EntityComponentHeaders/RenderComponent.h"
#include "EntityManager.h"

// Default Constructor:
//        Requires an EntityID for the Entity that the component is a part of
//            and a ComponentID issued by the EntityManager.
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
    glBindVertexArray(m_iVertexArray);
    glUseProgram(m_pShdrMngr->getProgram(m_eShaderType));

    // Bind Texture(s) HERE
    m_pMesh->bindTextures(m_eShaderType);

    // Call related glDraw function.
    if (m_bUsingInstanced)
        glDrawElementsInstanced(m_eMode, m_pMesh->getCount(), GL_UNSIGNED_INT, 0, m_pMesh->getNumInstances());
    else if (m_bUsingIndices)
        glDrawElements(m_eMode, m_iCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(m_eMode, 0, m_iCount);

    // Unbind Texture(s) HERE
    m_pMesh->unbindTextures();
}

// Overloaded Update Function
void RenderComponent::update(duration<float> fTimeDelta)
{
    /* Not Implemented */
}

void RenderComponent::initializeComponent(const Mesh* pMesh)
{
    // Ensure the Mesh passed in is valid.
    assert(nullptr != pMesh);

    // Get number of Vertices.
    m_iCount = pMesh->getCount();

    // Check Rendering Flags in Mesh.
    m_bUsingIndices = pMesh->usingIndices();
    m_bUsingInstanced = pMesh->usingInstanced();

    // Store Mesh for Reference.
    m_pMesh = pMesh;
    m_iVertexArray = pMesh->getVertexArray();    // Store Vertex Array Locally.
}
