#include "EntityHeaders/Camera.h"
#include "EntityManager.h"

/***********\
 * Defines *
\***********/
#define VERT_LOWER_LIMIT	1.f - FLT_EPSILON
#define VERT_UPPER_LIMIT	180.f - VERT_LOWER_LIMIT
#define HORIZONTAL_LIMIT	360.f
#define ZOOM_MIN			0.05f
#define FOV_Y				60.f
#define Z_CLOSE				0.01f
#define Z_FAR				10000.f
#define START_RADIUS		10.f

// Vector Indexing
#define I_THETA				0		// Spherical
#define I_PHI				1
#define I_RADIUS			2
#define I_X					0		// Cartesian
#define I_Y					1
#define I_Z					2

// Shortened Indexing
#define PHI					m_vPosition[I_PHI]
#define THETA				m_vPosition[I_THETA]
#define RADIUS				m_vPosition[I_RADIUS]
#define X_LOOK				m_vPosition[I_X]
#define Y_LOOK				m_vPosition[I_Y]
#define Z_LOOK				m_vPosition[I_Z]

const vec3 DEFAULT_POS = vec3(90.f, 8.5308f, START_RADIUS); // (Theta, Phi, Radius)

// Constructor
Camera::Camera( int iID )
	: Entity( iID, vec3(0.0f) )
{
	EntityManager* pEntMngr = ENTITY_MANAGER;
	m_bSteadyCam = false;
	m_pCmraCmp = pEntMngr->generateCameraComponent( iID );
	//m_pPhysicsCmp = pEntMngr->generatePhysicsComponent(iID); // PHYSICSTODO: Set this up for a camera to test Camera Physics.
	assert(m_pCmraCmp != NULL);
	m_vPosition = DEFAULT_POS;
	m_vWorldLookAt = vec3(0.f, 0.f, 0.f);	// (X,Y,Z)
	updateCameraComponent();
	m_pCmraCmp->setFOV_Y(FOV_Y);
	m_pCmraCmp->setZRange(Z_CLOSE, Z_FAR);
}

// Destructor
Camera::~Camera()
{
	m_pCmraCmp = NULL;
}

/// Camera Manipulation Functions
// Rotatable 360 degrees around.  Laps if it goes over that limit.
void Camera::orbit(vec2 pDelta)
{
	if (!m_bSteadyCam)
	{
		THETA += pDelta.x;
		THETA = THETA > HORIZONTAL_LIMIT ? THETA - HORIZONTAL_LIMIT : THETA;
		THETA = THETA < 0.f ? THETA + HORIZONTAL_LIMIT : THETA;

		PHI += pDelta.y;
		PHI = PHI < VERT_LOWER_LIMIT ? VERT_LOWER_LIMIT : PHI;
		PHI = PHI > VERT_UPPER_LIMIT ? VERT_UPPER_LIMIT : PHI;

		// Update Camera Component with new position
		updateCameraComponent();
		//cout << "CAMERA ORBIT: {" << THETA << ", " << PHI << "}" << endl;
	}
}

// Zoom in and out by a given Delta
void Camera::zoom(float fDelta)
{
	if (!m_bSteadyCam)
	{
		RADIUS -= fDelta * 25.0f;
		RADIUS = RADIUS < ZOOM_MIN ? ZOOM_MIN : RADIUS;
		RADIUS = RADIUS > ZOOM_MAX ? ZOOM_MAX : RADIUS;

		// Update Camera Component with new position.
		updateCameraComponent();
		//cout << "CAMERA ZOOM: {" << RADIUS << "}" << endl;
	}
}

// Private function to update information in Camera Component.
//	Called when values change.
void Camera::updateCameraComponent()
{
	m_pCmraCmp->setSphericalPos(m_vPosition);
	m_pCmraCmp->setLookAt(m_vWorldLookAt);
}





