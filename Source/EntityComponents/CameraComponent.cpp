#include "EntityComponentHeaders/CameraComponent.h"
#include "EntityManager.h"

/***********\
 * Defines *
\***********/
// Vector Indexing
#define I_THETA             0        // Spherical
#define I_PHI               1
#define I_RADIUS            2
#define I_X                 0        // Cartesian
#define I_Y                 1
#define I_Z                 2

// Shortened Indexing
#define PHI                 m_vPos[I_PHI]
#define THETA               m_vPos[I_THETA]
#define RADIUS              m_vPos[I_RADIUS]
#define X_LOOK              m_vPos[I_X]
#define Y_LOOK              m_vPos[I_Y]
#define Z_LOOK              m_vPos[I_Z]

// Default Camera Values
#define FOV_Y               60.f
#define Z_CLOSE             0.01f
#define Z_FAR               10000.f

/*************************************************************************\
 * Constructors                                                             *
\*************************************************************************/
// Default Constructor:
//        Requires an EntityID for the Entity that the component is a part of
//            and a ComponentID issued by the EntityManager.
CameraComponent::CameraComponent(int iEntityID, int iComponentID, int iHeight, int iWidth)
    : EntityComponent( iEntityID, iComponentID )
{
    updateHxW(iHeight, iWidth);
    m_m4Frame = mat4(1.0);
    setFOV_Y(FOV_Y);
    setZRange(Z_CLOSE, Z_FAR);
}

// Default Copy Constructor
CameraComponent::CameraComponent(const CameraComponent* pCopy)
    : EntityComponent(pCopy)
{
    // m_pRotation = quat();
}

// Default = operator overload; Private function
CameraComponent& CameraComponent::operator=(const CameraComponent* pCopy)
{
    return *this;
}

/*************************************************************************\
 * Destructor                                                             *
\*************************************************************************/
CameraComponent::~CameraComponent()
{

}

/*************************************************************************\
 * Inherited Functions                                                     *
\*************************************************************************/
// Inherited update function, not yet implemented.
void CameraComponent::update(float fTimeDeltaInMilliseconds)
{
    /* Not Implemented */
}

/*************************************************************************\
 * Getters                                                                 *
\*************************************************************************/

// Generates toCamera Matrix and updates Uniform in ShaderManager.
mat4 CameraComponent::getToCameraMat() const
{
    vec3 vCamCPos = getCartesianPos();
    return lookAt(vCamCPos, m_vWorldLookAt, vec3(m_m4Frame[1]));
}

// Generates toCamera Matrix and updates Uniform in ShaderManager.
mat4 CameraComponent::getPerspectiveMat() const
{
    return perspective(m_fFOV_Y * DEGREES_TO_RADIANS, m_fAspectRatio, m_fZClose, m_fZFar);
}

// fetches the World Position of the Camera
vec3 CameraComponent::getCameraWorldPos() const
{
    return getCartesianPos();
}

// Returns The Look At Vector for the Camera.
vec3 CameraComponent::getLookAt() const
{
    return m_vWorldLookAt - getCartesianPos();
}

// Returns the Current Camera Position in Cartesian Coordinates
vec3 CameraComponent::getCartesianPos() const
{
    // Locals - Fetch current position of Entity.
    float fPhi_Rads = PHI * DEGREES_TO_RADIANS;
    float fTheta_Rads = THETA * DEGREES_TO_RADIANS;
    vec3 vReturn;

    // Identity Matrix is a flag for basic spherical camera.
    if (m_m4Frame == mat4(1.0))
    {
        vReturn.z = RADIUS * sin(fPhi_Rads);    // Z = r·sinϕ
        vReturn.x = vReturn.z * sin(fTheta_Rads);        // use Z for X = r·sinϕ·sinθ
        vReturn.x = abs(vReturn.x) < FLT_EPSILON ? 0.f : vReturn.x;
        vReturn.z *= cos(fTheta_Rads);            // Finish Z: Z = r·sinϕ·cosθ
        vReturn.z = abs(vReturn.z) < FLT_EPSILON ? 0.f : vReturn.z;
        vReturn.y = RADIUS * cos(fPhi_Rads);    // Y: r·cosϕ
        vReturn.y = abs(vReturn.y) < FLT_EPSILON ? 0.f : vReturn.y;

        mat4 mLookAtTranslation = translate(mat4(1.f), m_vWorldLookAt);
        vec4 mTranslatedPos = mLookAtTranslation * (m_pRotation * vec4(vReturn, 1.f));
        vReturn = vec3(mTranslatedPos);
    }
    else // Defined for a specific camera behaviour, needs to be revised.
    {
        vec3 vTranslateBack = normalize(m_vWorldLookAt - vec3(m_m4Frame[3])) * -0.1;
        vTranslateBack += vec3(m_m4Frame[1] * 0.075);
        vReturn = vec3(translate(mat4(1.f), vTranslateBack) * m_m4Frame[3]);
    }

    return vReturn;
}

// Ray Casting:
//    Tutorial from: antongerdelan.net/opengl/raycasting.html
vec3 CameraComponent::getRay(float fX, float fY) const
{
    // Convert to 3D Normalized Device Coordinates
    float fMouseX = (2.0f * fX / (float)m_iWidth) - 1.0f;
    float fMouseY = (-2.0f * fY / (float)m_iHeight) + 1.0f;

    // Create 4D Homogeneous Clip Coordinates
    vec4 ray_clip = vec4(fMouseX, fMouseY, -1.0f, 1.0f);

    // 4D Eye (Camera) Coordinates - Inverse Projection Matrix
    vec4 ray_eye = inverse(getPerspectiveMat()) * ray_clip;

    // Un-project the x,y part -> manually set z,w
    ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

    // 4D World Coordinates
    return normalize(vec3(inverse(getToCameraMat()) * ray_eye));
}

// Handle logic for changing window size.
void CameraComponent::updateHxW(int iHeight, int iWidth)
{
    m_iHeight = iHeight;
    m_iWidth = iWidth;

    m_fAspectRatio = (float)m_iWidth / (float)m_iHeight;
}
