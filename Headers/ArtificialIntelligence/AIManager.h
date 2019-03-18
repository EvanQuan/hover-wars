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

    void initialize();

    void update(float fTimeInSeconds);
private:
    AIManager();
    static AIManager* m_pInstance;

    int lastStep = -1;
    // AIComponent *m_AIComponent;
    vector<AIComponent*> m_vAIComponents;
    // vector<glm::vec3> m_vTargetPositions;
    // vector<glm::vec3> m_vVelocities;

    void initializeAIComponent(HovercraftEntity* bot, AIComponent* ai);

};

