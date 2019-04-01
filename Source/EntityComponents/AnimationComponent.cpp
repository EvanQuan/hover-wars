#include "EntityComponentHeaders/AnimationComponent.h"
#include "EntityManager.h"
#include "EntityHeaders/Entity.h"

// DEFINES
#define ANIMATION_SPEED     (1.0f/60.0f)
#define CURR_FRAME          m_vKeyFrames[m_iCurrFrame]
#define NEXT_FRAME          m_vKeyFrames[m_iNextFrame]

// Default Constructor:
//        Requires an EntityID for the Entity that the component is a part of
//            and a ComponentID issued by the EntityManager.
AnimationComponent::AnimationComponent(int iEntityID, int iComponentID )
    : EntityComponent( iEntityID, iComponentID )
{
    m_fAnimTime             = 0.0f;
    m_fInterpolationK       = 0.0f;
    m_iCurrFrame            = m_iNextFrame = 0;
    m_sMeshInstanceHandle   = to_string(iEntityID) + " " + to_string(iComponentID);
    m_bAnimating            = false;
    m_m4WorldTransform      = mat4(1.0f);
}

// Destructor
AnimationComponent::~AnimationComponent()
{

}

// Base Update Function that branches based on Animation Component Type.
void AnimationComponent::update(float fTimeInSeconds)
{
    if (m_bBillboardAnimation)              // Update a Billboard Animation
        updateBillboard(fTimeInSeconds);
    else                                    // Update a regular Animation
        updateAnimation(fTimeInSeconds);
}

/*****************************************************************\
 * Private Functions                                             *
\*****************************************************************/

// Update a 3D Mesh Animation
void AnimationComponent::updateAnimation(float fTimeInSeconds)
{
    // Local Variables
    mat4 m4MeshTransform = mat4(1.0f);

    // Interpolate during an animation if there's more than one key frame
    if (m_bAnimating && (m_iCurrFrame != m_iNextFrame))
    {
        m_fAnimTime             -= fTimeInSeconds;                          // Update Animation Timer
        m_fInterpolationK       = m_fAnimTime / NEXT_FRAME.fTimeToKeyFrame; // Get Interpolation K
        m4MeshTransform = CURR_FRAME.interpolateWithKeyFrame(&NEXT_FRAME, m_fInterpolationK).toTransformationMatrix();
        m4MeshTransform = m_m4WorldTransform * m4MeshTransform;
        m_pMesh->updateInstance(&m4MeshTransform, m_sMeshInstanceHandle);

        // Update Animation if it's finished.
        if (0.0f >= m_fAnimTime)
        {
            m_bAnimating = false;
            setUpNextFrame();
        }
    }
    else if (1 <= m_vKeyFrames.size())
    {
        m4MeshTransform = m_m4WorldTransform * CURR_FRAME.toTransformationMatrix();
        m_pMesh->updateInstance(&m4MeshTransform, m_sMeshInstanceHandle);
    }
}

// Overloaded Update Function
void AnimationComponent::updateBillboard(float fTimeInSeconds)
{
    // Add Timestep to Animation GameTime.
    m_fAnimTime += fTimeInSeconds;
    bool bUpdateAnimation = m_fAnimTime >= ANIMATION_SPEED;
    bool bDeletionFlag = false;

    if (bUpdateAnimation)
        m_fAnimTime -= ANIMATION_SPEED;

    if (!m_pBillboardListPtr->empty())
    {
        // Update Billboards
        for( vector<Mesh::sBillboardInfo>::iterator iter = m_pMesh->m_pBillboardList.begin();
             iter != m_pMesh->m_pBillboardList.end();
             ++iter)
        {
            // Only compute Animation if it has duration remaining.
            if (iter->fDuration > 0.0f)
            {
                // Decrement Duration
                iter->fDuration -= fTimeInSeconds;
                bDeletionFlag |= iter->fDuration <= 0;

                // Animate the Sprite on the Animation Speed.
                if (bUpdateAnimation)
                {
                    // Move Sprite UVs right.
                    iter->vUVStart.x += m_vSpriteHxW.x;

                    // If overstepped past, reset back to beginning and evaluate v constraints
                    if (iter->vUVStart.x >= 1.0f)
                    {
                        // Reset u back to beginning.
                        iter->vUVStart.x = m_vSpriteHxWBorder.x;
                        iter->vUVEnd.x = m_vSpriteHxW.x - m_vSpriteHxWBorder.x;

                        // Evaluate v
                        iter->vUVStart.y += m_vSpriteHxW.y;

                        // If gone past bottom of sprite sheet, return back to beginning.
                        if (iter->vUVStart.y >= 1.0f)
                        {
                            iter->vUVStart.y = m_vSpriteHxWBorder.y;
                            iter->vUVEnd.y = m_vSpriteHxW.y - m_vSpriteHxWBorder.y;
                        }
                        else // No reset? update the end of the UV
                            iter->vUVEnd.y += m_vSpriteHxW.y;
                    }
                    else    // No reset? update the end of the UV
                        iter->vUVEnd.x += m_vSpriteHxW.x;
                }
            }
        }

        if (bDeletionFlag)
        {
            // Clean up any Billboards that are subject for deletion.
            m_pBillboardListPtr->erase(
                remove_if(
                    m_pBillboardListPtr->begin(),
                    m_pBillboardListPtr->end(),
                    [](Mesh::sBillboardInfo const & p) { return p.fDuration <= 0.f; }
                ),
                m_pBillboardListPtr->end());
        }

        // Update VBOs with Animated Sprite
        m_pMesh->updateBillboardVBO();
    }
}

// Set up New Current Frame and Next Frame
void AnimationComponent::setUpNextFrame()
{
    m_iCurrFrame = m_iNextFrame;
    m_iNextFrame = (m_iNextFrame + 1 >= m_vKeyFrames.size() ? 0 : m_iNextFrame + 1);
}

/*****************************************************************\
 * Keyframe Functionality                                        *
\*****************************************************************/

// Initializes The animation component simply with a Mesh
void AnimationComponent::initializeComponent(Mesh * pMesh)
{
    assert(nullptr != pMesh);
    m_pMesh = pMesh;
}

// Adds a Key Frame to the Animation Component 
void AnimationComponent::addKeyFrame(const vec3* vPosition,
                                     const quat* qRotation,
                                     float fScale, float fTimeToKeyFrame)
{
    sKeyFrame sNewKeyframe{*vPosition, *qRotation, fScale, fTimeToKeyFrame};
    m_vKeyFrames.push_back(sNewKeyframe);

    // If this is the first Keyframe added, store the Keyframe instance in the Mesh.
    if (1 == m_vKeyFrames.size())
    {
        // Get Transformation Matrix
        mat4 m4Transformation = m_m4WorldTransform * m_vKeyFrames.front().toTransformationMatrix();

        // Add Instance to the Mesh.
        m_pMesh->addInstance(&m4Transformation, m_sMeshInstanceHandle);
    }
    else if (m_iCurrFrame + 1 <= m_vKeyFrames.size())   // Set Next Frame
        m_iNextFrame = m_iCurrFrame + 1;
}

// Set up animation Component to Animate to the Next frame
void AnimationComponent::animateToNextFrame()
{
    if (m_bAnimating) // Interpolate to next frame instead
    {
        setUpNextFrame();
        m_fAnimTime     = NEXT_FRAME.fTimeToKeyFrame - (CURR_FRAME.fTimeToKeyFrame - m_fAnimTime);
    }
    else
    {
        m_fAnimTime     = NEXT_FRAME.fTimeToKeyFrame;
        m_bAnimating    = true;
    }
}

/*****************************************************************\
 * Billboard/Sprite Functionality                                *
\*****************************************************************/

unsigned int AnimationComponent::addBillboard(const vec3* vPosition, const vec3* vNormal)
{
    // Local Variables
    vec2 vUVRandStart = vec2(0.0f);
    vec2 vUVRandEnd = vec2(0.0f);

    // Get a Random starting Sprite for the billboard.
    pickRandomSprite(&vUVRandStart);
    vUVRandEnd = vUVRandStart + (m_vSpriteHxW - m_vSpriteHxWBorder);    // Calculate the End UV Coordinates of the random sprite
    vUVRandStart += m_vSpriteHxWBorder;                                 // Calculate the Beginning UV coordinates of the random sprite

    return m_pMesh->addBillboard(vPosition, vNormal, &vUVRandStart, &vUVRandEnd, m_fBillboardHeight, m_fBillboardWidth, m_fDuration);
}

void AnimationComponent::initializeComponentAsBillboard( Mesh* pMesh, const sSpriteSheetInfo* pSpriteInfo, float fBillboardHeight, float fBillboardWidth)
{
    // Ensure the Mesh passed in is valid.
    assert(nullptr != pMesh);

    // Store Billboard list and mesh for animation manipulation
    m_pBillboardListPtr = &pMesh->m_pBillboardList;
    m_bBillboardAnimation = true;
    m_pMesh = pMesh;

    // Store information about the sprite sheet and Billboard
    m_vSpriteHxW        = pSpriteInfo->vUVSpriteSize;
    m_vSpriteHxWBorder  = pSpriteInfo->vUVBorderSize;
    m_iNumSpritesX      = pSpriteInfo->iNumSpritesX;
    m_iNumSpritesY      = pSpriteInfo->iNumSpritesY;
    m_fDuration         = pSpriteInfo->fDuration;
    m_fBillboardHeight  = fBillboardHeight;
    m_fBillboardWidth   = fBillboardWidth;
}

/*****************************************************************\
 * Private Variables                                             *
\*****************************************************************/

// Convert the settings of a Keyframe to a transformation matrix for an instance of the mesh.
mat4 AnimationComponent::generateTransformationMatrix(const sKeyFrame* pKeyFrame )
{
    // Generate and return the Transformation Matrix:
    //  Translation * Rotation * Scale
    mat4 m4ReturnTransformation = scale(vec3(pKeyFrame->fScale));
    m4ReturnTransformation = toMat4(pKeyFrame->vRotation) * m4ReturnTransformation;
    return translate(pKeyFrame->vPosition) * m4ReturnTransformation;
}

// Picks a random sprite position (top left corner) based on the Texture HxW, the Sprite HxW and the Sprite buffer.
void AnimationComponent::pickRandomSprite(vec2* vReturnUV)
{
    // Local Variables
    int iRandX = FuncUtils::random(m_iNumSpritesX);   // Compute a Random index for the sprite sheet.
    int iRandY = FuncUtils::random(m_iNumSpritesY);   // Compute a Random index for the sprite sheet.

    // Convert those indices to their respective uv coordinates within the sprite sheet.
    vReturnUV->x = iRandX * m_vSpriteHxW.x;
    vReturnUV->y = iRandY * m_vSpriteHxW.y;
}

/*****************************************************************\
 * KeyFrame Functionality                                        *
\*****************************************************************/

// Interpolates between the called keyframe and another keyframe with a given k where 0.0f >= k <= 1.0f
AnimationComponent::sKeyFrame AnimationComponent::sKeyFrame::interpolateWithKeyFrame(const sKeyFrame* pOther, float fK)
{
    // Local Variables
    float fKInv = 1.0f - fK;
    sKeyFrame sReturnFrame{ *this };

    // Interpolate Position
    sReturnFrame.vPosition *= fK;
    sReturnFrame.vPosition += (pOther->vPosition * fKInv);

    // Interpolate Rotation
    sReturnFrame.vRotation *= fK;
    sReturnFrame.vRotation += (pOther->vRotation * fKInv);

    // Interpolate Scale
    sReturnFrame.fScale *= fK;
    sReturnFrame.fScale += (pOther->fScale * fKInv);

    // return result
    return sReturnFrame;
}

// Converts the keyframe to a transformation matrix for Meshes
mat4 AnimationComponent::sKeyFrame::toTransformationMatrix()
{
    // Generate and return the Transformation Matrix:
    //  Translation * Rotation * Scale
    mat4 m4ReturnTransformation = scale(vec3(fScale));
    m4ReturnTransformation = toMat4(vRotation) * m4ReturnTransformation;
    return translate(vPosition) * m4ReturnTransformation;
}
