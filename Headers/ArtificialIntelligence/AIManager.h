#pragma once
#include "EntityComponentHeaders/AIComponent.h"
#include "EntityHeaders/HovercraftEntity.h"
/*
@Deprecated This will be deleted once all AI related calculated are moved
outside of this class.
*/
class AIManager final
{
public:
    static AIManager* getInstance();

    ~AIManager();

    void reinitialize();

    void update(float fTimeInSeconds);
private:
    AIManager();
    static AIManager* m_pInstance;

    vector<AIComponent*> m_vAIComponents;

    void initializeAIComponent(HovercraftEntity* bot, AIComponent* ai);

};

