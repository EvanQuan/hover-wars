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

        @param aiType   to initialize for the given game
    */
    void reinitialize(eAIType aiType);

    void update(float fTimeInSeconds);

    eAIType getAIType() const { return m_eAIType; }
private:
    AIManager();

    static AIManager* m_pInstance;
    EntityManager* m_pEntityMngr;

    /*
    The unique pointers to these are deleted in EntityManager
    */
    vector<AIComponent*> m_vAIComponents;

    void initializeAIComponent(HovercraftEntity* bot, AIComponent* ai);

    void getNearestPlayer(const eHovercraft &bot,
                          const vec3 &botPosition,
                          eHovercraft &nearestPlayer,
                          float &distanceToPlayer);
    void getNearestBot(const eHovercraft &bot,
                       const vec3 &botPosition,
                       eHovercraft &nearestBot,
                       float &distanceToBot);
    void getNearestHovercraft(const vector<HovercraftEntity*>* hovercraftList,
                              const eHovercraft &bot, const vec3 &botPosition,
                              eHovercraft &nearestHovercraft,
                              float &distanceToHovercraft);

    eHovercraft getTargetID(eHovercraft nearestPlayer,
                            float distanceToPlayer,
                            eHovercraft nearestBot,
                            float distanceToBot);

    HovercraftEntity* getTarget(const eHovercraft &bot, const vec3 &botPosition);

    void executeAction(HovercraftEntity *bot, const Action &a);
    bool shouldMove(const HovercraftEntity *bot, const Action &a) const;

    eAIType m_eAIType;
};


