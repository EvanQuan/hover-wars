#pragma once
#include "MeshManager.h"
#include "TextureManager.h"

//////////////////////////////////////////////////////////////////
// Name: BoidEngine.h
// Class: Generates and Manages a series of Boids that animate over time.
// Written by: James Cote
//////////////////////////////////
class BoidEngine 
{
public:
	BoidEngine(const string* sModelFileName, const string* sTexFileName);
	~BoidEngine();

	// Public Functionality
	void draw( );
	void update();

private:
	BoidEngine(const BoidEngine* pCopy);
	BoidEngine* operator=(const BoidEngine& pRHS);

	// Scales the Boids
	float m_fScale;
	float m_fDeltaT;

	void initializeBoids();
	void genInstanceData();

	struct Boid {
		vec3 vPos, vVelocity, vForce;
		float fAvoidDistance, fCohesionDistance, fGatherDistance;
		mat4 m4FrenetFrame;

		// Calculates Acceleration and updates Velocity and Position
		//  Resets Force afterwards.
		void integrate(float fDeltaT)
		{
			// Integrate Boid
			vVelocity += vForce * fDeltaT;
			vPos += vVelocity * fDeltaT;

			// Setup Frenet Frame
			m4FrenetFrame[2] = vec4(normalize(vVelocity), 0.0f);																	// Tangent
			m4FrenetFrame[0] = vec4(normalize(cross(vForce + vec3(0.0f, 100.0f, 0.0f)/*UP-VEC*/, vec3(m4FrenetFrame[2]))), 0.0f);	// BiNormal
			m4FrenetFrame[1] = vec4(normalize(cross(vec3(m4FrenetFrame[2]), vec3(m4FrenetFrame[0]))), 0.0f);						// Normal
			m4FrenetFrame[3] = vec4(vPos, 1.0f);
			vForce = vec3(0.0f);
		}
	};

	vector< Boid > m_vBoids;
	vector< mat4 > m_vInstanceData;

	Mesh* m_pMesh;
	Texture* m_pTexture;
	quat m_pQuaternion;
};

