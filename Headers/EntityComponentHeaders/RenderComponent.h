#pragma once
#include "stdafx.h"
#include "EntityComponent.h"
#include "ShaderManager.h"

/************************************************************
* Name: RenderComponent
* Written by: James Cote
* Description: Component used for storing vertex information and handles
*    for the GPU to render to the screen.
***************************************************************/

// Forward Declarations
class EntityManager;
class Mesh;

class RenderComponent
    : public EntityComponent
{
public:
    RenderComponent(int iEntityID, int iComponentID, bool bRenderShadows,
                    ShaderManager::eShaderType eType, GLenum eMode);
    virtual ~RenderComponent();

    // render will call the GPU to draw the bound data.
    void render();

    // Inherited update frunction from EntityComponent
    void update(float fTimeInSeconds);
    void updateCount(GLsizei iNewCount) { m_iCount = iNewCount; }

    // Initializes the proper buffers on the GPU for rendering.
    void initializeComponent( Mesh const  * pMesh );

    // Check to see if this render component should be rendered for shadows.
    bool castsShadows() { return m_bRenderShadows; }

private:
    // Private Copy Constructor and Assignment operator overload.
    RenderComponent(const RenderComponent* pCopy);
    RenderComponent& operator=(const RenderComponent& pRHS);

    // Private Variables
    GLenum m_eMode;
    GLsizei m_iCount;
    Mesh const * m_pMesh;
    bool m_bUsingIndices, m_bUsingInstanced, m_bRenderShadows;
    ShaderManager* m_pShdrMngr;
    EntityManager* m_pEntityManager;
    ShaderManager::eShaderType m_eShaderType;
};
