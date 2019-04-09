#pragma once
#include "EntityHeaders/Entity.h"
#include "EntityComponentHeaders/RenderComponent.h"

/*************************************************
 * Name: Emitter
 * Description: Maintains data for Emitter such as
 *              Number, position and duration of particles
\**************************************************/
class Emitter
{
public:
    // Default Constructor and Destructor.
    Emitter(const vec3* vPosition);
    virtual ~Emitter();

    // Functions to initialize and update Emitter.
    void initializeEmitter(unsigned int iMaxParticles, vec3 vEmitterNormal, float fAngleFromNormal, float fDefaultDuration, float fRadius, bool bExplosion);
    bool update(float fDelta);
    void draw();

    // Function to signal EmitterEngine that Emitter is finished.
    bool readyToDelete() const { return m_bReadyToDelete; }

private:
    // Internal Emitter Variables
    unsigned int m_iMaxNumParticles, m_iCurrNumParticles;
    vec3 m_vPosition, m_vEmitterNormal, m_vOrthogonalVec;
    float m_fAngleFromNormal, m_fDefaultDuration, m_fRadius;
    bool m_bReadyToDelete;
    GLuint m_iVertexArray, m_iVertexBuffer;
    // Particle information
    struct sParticleInformation
    {
        vec3 vPosition, vVelocity, vForce;
        float fDuration;
    };

    // List of Particles
    vector<unique_ptr<sParticleInformation>> m_pParticleList;
    vector< vec3 > m_pPositions;

    Texture*    m_pDiffuseTexture;
    Mesh*       m_pBillboardMesh;

    // Private Functions
    void spawnNewParticle();
};

