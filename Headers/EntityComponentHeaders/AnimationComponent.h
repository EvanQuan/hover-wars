#pragma once
#include "stdafx.h"
#include "EntityComponent.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "DataStructures/SpriteSheetDatabase.h"

using namespace SpriteSheetDatabase;

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
    void update(float fTimeInSeconds);

    // Allows the user to add a billboard at a given position.
    unsigned int addBillboard(const vec3* vPosition, const vec3* vNormal);

    // Initializes the proper buffers on the GPU for rendering.
    void initializeComponentAsBillboard( Mesh * pMesh, const sSpriteSheetInfo* pSpriteInfo, float fBillboardHeight, float fBillboardWidth );
    void initializeComponent(Mesh * pMesh);
    void addKeyFrame(const vec3* vPosition, const quat* qRotation, float fScale, float fTimeToKeyFrame);
    void animateToNextFrame();

private: 
    // Private Copy Constructor and Assignment operator overload.
    AnimationComponent(const AnimationComponent* pCopy);
    AnimationComponent& operator=(const AnimationComponent& pRHS);

    void pickRandomSprite(vec2* vReturnUV);
    void updateBillboard(float fTimeInSeconds);
    void updateAnimation(float fTimeInSeconds);

    // Keyframe Animation
    struct sKeyFrame
    {
        vec3 vPosition;
        quat vRotation;
        float fScale, fTimeToKeyFrame;

        // Function to Linearly Interpolate between two keyframes
        sKeyFrame interpolateWithKeyFrame(const sKeyFrame* pOther, float fK);
        mat4 toTransformationMatrix();
    };
    vector<sKeyFrame> m_vKeyFrames;
    unsigned int m_iCurrFrame, m_iNextFrame;
    float m_fInterpolationK;
    bool m_bBillboardAnimation, m_bAnimating;
    string m_sMeshInstanceHandle;
    mat4 generateTransformationMatrix(const sKeyFrame* pKeyFrame);
    void setUpNextFrame();

    // Private Variables
    Mesh * m_pMesh;
    vector<Mesh::sBillboardInfo>*   m_pBillboardListPtr;
    vector<unsigned int>            m_pBillboardIndexCleanList;

    // Sprite HxW is specified as the full sprite width (texture width / num sprites per
    vec2 m_vSpriteHxW, m_vSpriteHxWBorder;
    int m_iNumSpritesX, m_iNumSpritesY;
    float m_fBillboardHeight, m_fBillboardWidth;
    float m_fAnimTime, m_fDuration;
};
