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
                    float fScale);

    void update(float fTimeInSeconds);
private:
    int lastStep = -1;
    AIComponent *m_AIComponent;
    glm::vec3 playerPos;
    glm::vec3 playerVel;
};

