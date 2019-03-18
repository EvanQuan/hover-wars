#pragma once
#include "EntityComponentHeaders/AIComponent.h"
#include "EntityHeaders/HovercraftEntity.h"

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

