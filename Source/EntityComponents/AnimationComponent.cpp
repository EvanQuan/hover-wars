#include "EntityComponentHeaders/AnimationComponent.h"
#include "EntityManager.h"

// DEFINES
#define ANIMATION_SPEED (1.0f/60.0f)

// Default Constructor:
//        Requires an EntityID for the Entity that the component is a part of
//            and a ComponentID issued by the EntityManager.
AnimationComponent::AnimationComponent(int iEntityID, int iComponentID )
    : EntityComponent( iEntityID, iComponentID )
{
    m_fAnimTime = 0.0f;
}

// Destructor
AnimationComponent::~AnimationComponent()
{

}

// Overloaded Update Function
void AnimationComponent::update(float fTimeInSeconds)
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

// Picks a random sprite position (top left corner) based on the Texture HxW, the Sprite HxW and the Sprite buffer.
void AnimationComponent::pickRandomSprite(vec2* vReturnUV)
{
    // Local Variables
    int iRandX = rand() % m_iNumSpritesX;   // Compute a Random index for the sprite sheet.
    int iRandY = rand() % m_iNumSpritesY;   // Compute a Random index for the sprite sheet.

    // Convert those indices to their respective uv coordinates within the sprite sheet.
    vReturnUV->x = iRandX * m_vSpriteHxW.x;
    vReturnUV->y = iRandY * m_vSpriteHxW.y;
}
