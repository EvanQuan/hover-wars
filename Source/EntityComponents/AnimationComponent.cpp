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
void AnimationComponent::update(float fTimeDeltaInMilliseconds)
{
    // Add Timestep to Animation Time.
    m_fAnimTime += fTimeDeltaInMilliseconds;
    bool bDeletionFlag = false;

    if (!m_pBillboardListPtr->empty())
    {
        // Update Billboards
        for( unsigned int i = 0; i < m_pBillboardListPtr->size(); ++i )
        {
            // Only compute Animation if it has duration remaining.
            if ((*m_pBillboardListPtr)[i].fDuration > 0.0f)
            {
                // Decrement Duration
                (*m_pBillboardListPtr)[i].fDuration -= fTimeDeltaInMilliseconds;
                bDeletionFlag |= (*m_pBillboardListPtr)[i].fDuration <= 0;

                // Animate the Sprite on the Animation Speed.
                if (m_fAnimTime >= ANIMATION_SPEED)
                {
                    m_fAnimTime -= ANIMATION_SPEED;

                    // Move Sprite UVs right.
                    (*m_pBillboardListPtr)[i].vUVStart.x += m_vSpriteHxW.x;

                    // If overstepped past, reset back to beginning and evaluate v constraints
                    if ((*m_pBillboardListPtr)[i].vUVStart.x >= 1.0f)
                    {
                        // Reset u back to beginning.
                        (*m_pBillboardListPtr)[i].vUVStart.x = m_vSpriteHxWBorder.x;
                        (*m_pBillboardListPtr)[i].vUVEnd.x = m_vSpriteHxW.x - m_vSpriteHxWBorder.x;

                        // Evaluate v
                        (*m_pBillboardListPtr)[i].vUVStart.y += m_vSpriteHxW.y;

                        // If gone past bottom of sprite sheet, return back to beginning.
                        if ((*m_pBillboardListPtr)[i].vUVStart.y >= 1.0f)
                        {
                            (*m_pBillboardListPtr)[i].vUVStart.y = m_vSpriteHxWBorder.y;
                            (*m_pBillboardListPtr)[i].vUVEnd.y = m_vSpriteHxW.y - m_vSpriteHxWBorder.y;
                        }
                        else // No reset? update the end of the UV
                            (*m_pBillboardListPtr)[i].vUVEnd.y += m_vSpriteHxW.y;
                    }
                    else    // No reset? update the end of the UV
                        (*m_pBillboardListPtr)[i].vUVEnd.x += m_vSpriteHxW.x;
                }

                m_pMesh->updateBillboardVBO(i);
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

            m_pMesh->updateBillboardVBO();
        }
    }
}

void AnimationComponent::addBillboard(const vec3* vPosition, const vec3* vNormal)
{
    // Local Variables
    vec2 vUVRandStart = vec2(0.0f);
    vec2 vUVRandEnd = vec2(0.0f);

    // Get a Random starting Sprite for the billboard.
    pickRandomSprite(&vUVRandStart);
    vUVRandEnd = vUVRandStart + (m_vSpriteHxW - m_vSpriteHxWBorder);    // Calculate the End UV Coordinates of the random sprite
    vUVRandStart += m_vSpriteHxWBorder;                                 // Calculate the Beginning UV coordinates of the random sprite

    m_pMesh->addBillboard(vPosition, vNormal, &vUVRandStart, &vUVRandEnd, m_fBillboardHeight, m_fBillboardWidth, m_fDuration);
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
