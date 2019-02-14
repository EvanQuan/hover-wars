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

    // Evaluate Current Animation time.
    while (m_fAnimTime >= ANIMATION_SPEED)
    {
        // Subtract Animation Speed.
        m_fAnimTime -= ANIMATION_SPEED;

        for (vector<Mesh::sBillboardInfo>::iterator iter = m_pBillboardListPtr->begin();
            iter != m_pBillboardListPtr->end();
            ++iter)
        {
            // Move Sprite UVs right.
            iter->vUVStart.x += m_vSpriteHxW.x;
            iter->vUVEnd.x += m_vSpriteHxW.x;

            // If overstepped past, reset back to beginning and evaluate v constraints
            if (iter->vUVStart.x >= 1.0f)
            {
                // Reset u back to beginning.
                iter->vUVStart.x -= 1.0f;
                iter->vUVEnd.x -= 1.0f;

                // Evaluate v
                iter->vUVStart.y += m_vSpriteHxW.y;
                iter->vUVEnd.y += m_vSpriteHxW.y;

                // If gone past bottom of sprite sheet, return back to beginning.
                if (iter->vUVStart.y >= 1.0f)
                {
                    iter->vUVStart.y -= 1.0f;
                    iter->vUVEnd.y -= 1.0f;
                }
            }
        }

        // Update VBOs for Mesh.
        m_pMesh->updateBillboardUVs();
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

    m_pMesh->addBillboard(vPosition, vNormal, &vUVRandStart, &vUVRandEnd, m_fBillboardHeight, m_fBillboardWidth);
}

void AnimationComponent::initializeComponentAsBillboard( Mesh* pMesh, vec2 vSpriteHxW, vec2 vSpriteHxWBorder,
                                            int iNumSpritesX, int iNumSpritesY, float fBillboardHeight, float fBillboardWidth)
{
    // Ensure the Mesh passed in is valid.
    assert(nullptr != pMesh);

    // Store Billboard list and mesh for animation manipulation
    m_pBillboardListPtr = &pMesh->m_pBillboardList;
    m_pMesh = pMesh;

    // Store information about the sprite sheet and Billboard
    m_vSpriteHxW        = vSpriteHxW;
    m_vSpriteHxWBorder  = vSpriteHxWBorder;
    m_iNumSpritesX      = iNumSpritesX;
    m_iNumSpritesY      = iNumSpritesY;
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
