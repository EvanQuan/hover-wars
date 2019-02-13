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

void AnimationComponent::initializeComponent( Mesh* pMesh, vec2 vSpriteHxW, vec2 vSpriteHxWBuffer )
{
    // Ensure the Mesh passed in is valid.
    assert(nullptr != pMesh);

    // Store Billboard list for updating.
    m_pBillboardListPtr = &pMesh->m_pBillboardList;

    m_vSpriteHxW = vSpriteHxW;
    m_vSpriteHxWBuffer = vSpriteHxWBuffer;
}
