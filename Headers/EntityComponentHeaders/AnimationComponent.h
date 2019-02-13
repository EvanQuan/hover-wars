#pragma once
#include "stdafx.h"
#include "EntityComponent.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "TextureManager.h"

/************************************************************
* Name: AnimationComponent
* Written by: James Cote
* Description: Component used for storing vertex information and handles
*    for the GPU to render to the screen.
***************************************************************/

class AnimationComponent 
    : public EntityComponent
{
public:
    AnimationComponent(int iEntityID, int iComponentID);
    virtual ~AnimationComponent();

    // Inherited update frunction from EntityComponent
    void update(duration<float> fTimeDelta);

    // Initializes the proper buffers on the GPU for rendering.
    void initializeComponent( Mesh  * pMesh, vec2 vSpriteHxW, vec2 vSpriteHxWBuffer);

private: 
    // Private Copy Constructor and Assignment operator overload.
    AnimationComponent(const AnimationComponent* pCopy);
    AnimationComponent& operator=(const AnimationComponent& pRHS);

    // Private Variables
    Mesh * m_pMesh;
    vector<Mesh::sBillboardInfo>* m_pBillboardListPtr;
    vec2 m_vSpriteHxW, m_vSpriteHxWBuffer;
};
