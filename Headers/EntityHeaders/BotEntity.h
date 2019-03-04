#pragma once
#include "EntityHeaders/HovercraftEntity.h"
#include "EntityComponentHeaders/AIComponent.h"
class BotEntity :
    public HovercraftEntity
{
public:
    BotEntity(int iID, const vec3* vPosition);
    virtual ~BotEntity();

    void initialize(const string& sFileName,
                    const ObjectInfo* pObjectProperties,
                    const string& sShaderType,
                    float fScale,
                    unsigned int iStatsID);

    // Overridden virtual functions inherited from parent
    void hit(eEntityTypes eHitByType, unsigned int iNumber);
    // void hit(eEntityTypes eHitByType, unsigned int iNumber) const;
    void update(float fTimeInMilliseconds);
private:
    int lastStep = -1;
    void toEulerAngle(glm::quat q, double& roll, double& pitch, double& yaw);
    AIComponent *m_AIComponent;
    glm::vec3 playerPos;
    glm::vec3 playerVel;
};

