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
    
}

// Destructor
AnimationComponent::~AnimationComponent()
{

}

// Overloaded Update Function
void AnimationComponent::update(duration<float> fTimeDelta)
{
    
}

void AnimationComponent::initializeComponent( Mesh* pMesh, vec2 vSpriteHxW, vec2 vSpriteHxWBorder, int iNumSpritesX, int iNumSpritesY )
{
    // Ensure the Mesh passed in is valid.
    assert(nullptr != pMesh);

    // Store Billboard list and mesh for animation manipulation
    m_pBillboardListPtr = &pMesh->m_pBillboardList;
    m_pMesh = pMesh;

    // Store information about the sprite sheet
    m_vSpriteHxW = vSpriteHxW;
    m_vSpriteHxWBorder = vSpriteHxWBorder;
    m_iNumSpritesX = iNumSpritesX;
    m_iNumSpritesY = iNumSpritesY;
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
