#pragma once
#include "EntityHeaders/HovercraftEntity.h"
#include "EntityComponentHeaders/AIComponent.h"
/*
@Deprecated This will be deleted once all AI related calculated are moved
outside of this class.
*/
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
                    eHovercraft eHovercraftID);

    // Overridden virtual functions inherited from parent
    void getHitBy(eEntityType eHitByType, unsigned int iNumber);
    // void hit(eEntityType eHitByType, unsigned int iNumber) const;
    void update(float fTimeInMilliseconds);
private:
    int lastStep = -1;
    void toEulerAngle(glm::quat q, double& roll, double& pitch, double& yaw);
    AIComponent *m_AIComponent;
    glm::vec3 playerPos;
    glm::vec3 playerVel;
};

