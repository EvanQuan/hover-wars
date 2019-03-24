#pragma once
#include "EntityComponentHeaders/AIComponent.h"
#include "EntityHeaders/HovercraftEntity.h"

// Forward Declarations
class EntityManager;

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
    EntityManager* m_pEntityMngr;

    vector<AIComponent*> m_vAIComponents;

    void initializeAIComponent(HovercraftEntity* bot, AIComponent* ai);
};

