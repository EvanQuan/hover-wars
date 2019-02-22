#include "Emitter.h"
#include <glm/gtc/random.hpp>
#include "ShaderManager.h"
#include "TextureManager.h"

/*************\
 * Constants *
\*************/
const vec3 GRAVITY = vec3(0.0f, -9.81f, 0.0f); 
const vec4 COLOR = vec4(1.f, 0.f, 0.f, 1.0f);

// Default Constructor
Emitter::Emitter(const vec3* vPosition)
{
    m_iMaxNumParticles = 0;
    m_iCurrNumParticles = 0;
    m_vPosition = *vPosition;
    m_vEmitterNormal = vec3(0.f, 1.f, 0.f);
    m_vOrthogonalVec = vec3(1.f, 0.f, 0.f);
    m_fAngleFromNormal = m_fDefaultDuration = 0.f;
    m_bReadyToDelete = false;
    glGenVertexArrays(1, &m_iVertexArray);
}

// Destructor: Eliminate Particles and delete Render Component.
Emitter::~Emitter()
{
    m_pParticleList.clear();
    glDeleteBuffers(1, &m_iVertexBuffer);
    glDeleteVertexArrays(1, &m_iVertexArray);
}

/**************************************************************************************\
 * Functions to initialize and update Emitter.                                        *
\**************************************************************************************/

// Initializes the Emitter 
void Emitter::initializeEmitter(unsigned int iMaxParticles, 
                                vec3 vEmitterNormal, 
                                float fAngleFromNormal, 
                                float fDefaultDuration, float fRadius, bool bExplosion)
{
    // Set internal variables
    m_iMaxNumParticles = iMaxParticles;
    m_pParticleList.reserve(iMaxParticles);    // Reserve maximum size to avoid resizing computation
    m_pPositions.reserve(iMaxParticles);    // Reserve maximum size to avoid resizing computation
    m_iCurrNumParticles = 0;
    m_vEmitterNormal = vEmitterNormal;
    m_fAngleFromNormal = fAngleFromNormal;
    m_fDefaultDuration = fDefaultDuration;
    m_fRadius = fRadius;
    unsigned int m = 0, n = 0;
    vec3 m_vOrthogonalVec = vec3(0.0);

    // Find some orthogonal coordinates for Emitter
    // Find index m s.t. Xm != 0
    while (numeric_limits<float>::epsilon() >= fabs(m_vEmitterNormal[m]))
        ++m;

    // Pick any other index n != m
    n = (m + 1) % 3;

    // Grab a vector Orthogonal to the normal.
    m_vOrthogonalVec[n] = m_vEmitterNormal[m];
    m_vOrthogonalVec[m] = -m_vEmitterNormal[n];
    m_vOrthogonalVec = normalize(m_vOrthogonalVec);

    // Create first particle
    if (bExplosion)
    {
        for (unsigned int i = 0; i < m_iMaxNumParticles; ++i)
            spawnNewParticle();
    }
    
    // Set up Vertex Buffer.
    m_iVertexBuffer = SHADER_MANAGER->genVertexBuffer(m_iVertexArray, m_pPositions.data(), m_pPositions.size() * sizeof(vec3), GL_DYNAMIC_DRAW);
    SHADER_MANAGER->setAttrib(m_iVertexArray, 0, 3, 0, (void*)0);

    m_pDiffuseTexture = TEXTURE_MANAGER->genTexture(&COLOR);
}

// Integrate Particle Positions, velocities and forces as well as updating the remaining duration for
//    all particles.
bool Emitter::update(float fDelta)
{
    // Store GameTime as float
    m_bReadyToDelete = true;
    m_pPositions.resize(m_pParticleList.size(), vec3(0.f));

    // Integrate Particles
    for (unsigned int i = 0; i < m_pParticleList.size(); ++i)
    {
        if (m_pParticleList[i]->fDuration > 0.f)
        {
            m_bReadyToDelete = false;
            m_pParticleList[i]->vVelocity += m_pParticleList[i]->vForce * fDelta;
            m_pParticleList[i]->vForce += GRAVITY * fDelta;
            m_pParticleList[i]->vPosition += m_pParticleList[i]->vVelocity * fDelta;
            m_pParticleList[i]->fDuration -= fDelta;
            m_pPositions[i] = m_pParticleList[i]->vPosition;
        }
    }

    // Clean up any Emitters that are subject for deletion.
    m_pParticleList.erase(
        remove_if(
            m_pParticleList.begin(),
            m_pParticleList.end(),
            [](unique_ptr<sParticleInformation> const & p) { return p->fDuration <= 0.f; }
        ),
        m_pParticleList.end());

    // Spawn a new one if available.
    if (m_iCurrNumParticles != m_iMaxNumParticles)
    {
        m_bReadyToDelete = false;
        spawnNewParticle();
    }

    // Return boolean saying that the emitter is ready to delete
    return m_bReadyToDelete;
}

// Draw the Particles from the Emitter
void Emitter::draw()
{
    // Update Vertex Buffer with new Positions
    glBindVertexArray(m_iVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_pPositions.size() * sizeof(vec3), m_pPositions.data(), GL_DYNAMIC_DRAW);
    glUseProgram(SHADER_MANAGER->getProgram(ShaderManager::eShaderType::PARTICLE_SHDR));

    m_pDiffuseTexture->bindTexture(ShaderManager::eShaderType::PARTICLE_SHDR, "sMaterial.vDiffuse");

    glPointSize(10.f);
    glDrawArrays(GL_POINTS, 0, m_pPositions.size());
    glPointSize(1.f);

    m_pDiffuseTexture->unbindTexture();

}

/************************************************************************************\
 * Private Variables                                                                *
\************************************************************************************/

// Generates a new Particle, currently spawns at the position of the emitter with a random
//    vector within the spherical sector defined by the emitter normal and the angle from normal.
//    2 random angles are generated and created into quaternions around the Orthogonal and Normal vectors
//    A copy of the normal vector is rotated by both quaternions respectively to create a new
//    random vector.
void Emitter::spawnNewParticle()
{
    // Generate Random Direction
    float fRandPhi = linearRand(0.f, m_fAngleFromNormal * DEGREES_TO_RADIANS);
    float fRandTheta = linearRand(0.f, PI_2);

    quat qRotateWRTOrthogonal    = angleAxis(fRandPhi, m_vOrthogonalVec);
    quat qRotateWRTNormal        = angleAxis(fRandTheta, m_vEmitterNormal);

    // Generate Particle information
    unique_ptr<sParticleInformation> pNewParticle = make_unique<sParticleInformation>();
    pNewParticle->vVelocity = vec3(qRotateWRTNormal * (qRotateWRTOrthogonal * vec4(m_vEmitterNormal, 1.0f)));
    pNewParticle->vVelocity *= m_fRadius; // Initial force equal to radius
    pNewParticle->vForce = pNewParticle->vVelocity;
    pNewParticle->fDuration = m_fDefaultDuration;
    pNewParticle->vPosition = m_vPosition;

    // Store Particle.
    m_pParticleList.push_back(move(pNewParticle));

    // Count new Particle
    ++m_iCurrNumParticles;
}
