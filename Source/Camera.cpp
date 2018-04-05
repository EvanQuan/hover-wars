#include "Camera.h"

/***********\
* Defines *
\***********/
#define PI					3.14159265f
#define VERT_LOWER_LIMIT	1.f - FLT_EPSILON
#define VERT_UPPER_LIMIT	180.f - VERT_LOWER_LIMIT
#define HORIZONTAL_LIMIT	360.f
#define ZOOM_MIN			0.05f
#define FOV_Y				60.f
#define Z_CLOSE				0.01f
#define Z_FAR				10000.f
#define START_RADIUS		2.5f

// Vector Indexing
#define I_THETA				0		// Spherical
#define I_PHI				1
#define I_RADIUS			2
#define I_X					0		// Cartesian
#define I_Y					1
#define I_Z					2

// Shortened Indexing
#define PHI					m_vSPos[I_PHI]
#define THETA				m_vSPos[I_THETA]
#define RADIUS				m_vSPos[I_RADIUS]
#define X_LOOK				m_vSPos[I_X]
#define Y_LOOK				m_vSPos[I_Y]
#define Z_LOOK				m_vSPos[I_Z]

const vec3 DEFAULT_POS = vec3( 90.f, 8.5308f, START_RADIUS ); // (Theta, Phi, Radius)

// Constructor
Camera::Camera( int iHeight, int iWidth )
{
	m_vSPos = DEFAULT_POS;	
	m_vWorldLookAt = vec3( 0.f, 0.f, 0.f );		// (X, Y, Z)
	updateHxW( iHeight, iWidth );
	m_m4Frame = mat4( 1.0 );

	m_bUpdated = true;
	m_bSteady = false;
}

// Copy Constructor
Camera::Camera( Camera& pCopy )
{
	m_vSPos = pCopy.m_vSPos;
	m_vWorldLookAt = pCopy.m_vWorldLookAt;
	m_m4Frame = pCopy.m_m4Frame;
	updateHxW( pCopy.m_iHeight, pCopy.m_iWidth );
	m_bUpdated = pCopy.m_bUpdated;
	m_bSteady = pCopy.m_bSteady;
}

// Destructor
Camera::~Camera()
{
	// Nothing to Destruct
}

// Generates toCamera Matrix and updates Uniform in ShaderManager.
mat4 Camera::getToCameraMat()
{
	vec3 vCamCPos = getCartesianPos();
	return lookAt( vCamCPos, m_vWorldLookAt, vec3( m_m4Frame[1] ) );
}

// Generates toCamera Matrix and updates Uniform in ShaderManager.
mat4 Camera::getPerspectiveMat()
{
	//vec3 vCamCPos = getCartesianPos();
	return perspective( FOV_Y * PI / 180.f, m_fAspectRatio, Z_CLOSE, Z_FAR );
}

// fetches the World Position of the Camera
vec3 Camera::getCameraWorldPos()
{
	return getCartesianPos();
}

// Returns The Look At Vector for the Camera.
const vec3 Camera::getLookAt()
{
	return m_vWorldLookAt - getCartesianPos();
}

// Returns the Current Camera Position in Cartesian Coordinates
vec3 Camera::getCartesianPos()
{
	float fPhi_Rads = PHI * PI / 180.f;
	float fTheta_Rads = THETA * PI / 180.f;
	vec3 vReturn;

	if ( m_m4Frame == mat4( 1.0 ) )
	{
		vReturn.z = RADIUS * sin( fPhi_Rads );	// Z = r·sinϕ
		vReturn.x = vReturn.z * sin( fTheta_Rads );		// use Z for X = r·sinϕ·sinθ
		vReturn.x = abs( vReturn.x ) < FLT_EPSILON ? 0.f : vReturn.x;
		vReturn.z *= cos( fTheta_Rads );			// Finish Z: Z = r·sinϕ·cosθ
		vReturn.z = abs( vReturn.z ) < FLT_EPSILON ? 0.f : vReturn.z;
		vReturn.y = RADIUS * cos( fPhi_Rads );	// Y: r·cosϕ
		vReturn.y = abs( vReturn.y ) < FLT_EPSILON ? 0.f : vReturn.y;
	
		mat4 mLookAtTranslation = translate( mat4( 1.f ), m_vWorldLookAt );
		vec4 mTranslatedPos = mLookAtTranslation * vec4( vReturn, 1.f );
		vReturn = vec3( mTranslatedPos );
	}
	else
	{
		vec3 vTranslateBack = normalize( m_vWorldLookAt - vec3( m_m4Frame[ 3 ] ) ) * -0.1;
		vTranslateBack += vec3( m_m4Frame[1] * 0.075 );
		vReturn = vec3( translate( mat4( 1.f ), vTranslateBack ) * m_m4Frame[ 3 ] );	
	}

	

	return vReturn;
}

// Handle logic for changing window size.
void Camera::updateHxW( int iHeight, int iWidth )
{
	m_iHeight = iHeight;
	m_iWidth = iWidth;

	m_fAspectRatio = (float)m_iWidth / (float)m_iHeight;
}

/// Camera Manipulation Functions
// Rotatable 360 degrees around.  Laps if it goes over that limit.
void Camera::orbit( vec2 pDelta )
{
	if ( !m_bSteady )
	{
		THETA += pDelta.x;
		THETA = THETA > HORIZONTAL_LIMIT ? THETA - HORIZONTAL_LIMIT : THETA;
		THETA = THETA < 0.f ? THETA + HORIZONTAL_LIMIT : THETA;

		PHI += pDelta.y;
		PHI = PHI < VERT_LOWER_LIMIT ? VERT_LOWER_LIMIT : PHI;
		PHI = PHI > VERT_UPPER_LIMIT ? VERT_UPPER_LIMIT : PHI;

		//cout << "CAMERA ORBIT: {" << THETA << ", " << PHI << "}" << endl;
	}
}

// Zoom in and out by a given Delta
void Camera::zoom( float fDelta )
{
	
	RADIUS -= fDelta;
	RADIUS = RADIUS < ZOOM_MIN ? ZOOM_MIN : RADIUS;
	RADIUS = RADIUS > ZOOM_MAX ? ZOOM_MAX : RADIUS;

	//cout << "CAMERA ZOOM: {" << RADIUS << "}" << endl;
}
