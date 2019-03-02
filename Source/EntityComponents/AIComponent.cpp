#include "EntityComponentHeaders/AIComponent.h"


AIComponent::AIComponent(int iEntityID, int iComponentID) : EntityComponent(iEntityID, iComponentID)
{

}
AIComponent::~AIComponent() {
    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
        delete frames[i];
    }
}
void AIComponent::update(float fTimeDeltaInMilliseconds)
{
    for (int i = 0; i < LOOK_AHEAD_FRAMES; i++) {
        Action *currAction = new Action;
        frames.push_back(currAction);
    }
}
