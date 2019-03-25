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

    /*
        Initialize everything that's needed for the start of a new game.
        This should be called after all bot entities have been created.

    */
    void reinitialize();

    void update(float fTimeInSeconds);
private:
    AIManager();

    void clearAIComponents();
    static AIManager* m_pInstance;
    EntityManager* m_pEntityMngr;

    vector<AIComponent*> m_vAIComponents;

    void initializeAIComponent(HovercraftEntity* bot, AIComponent* ai);
};

