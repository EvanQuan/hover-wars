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
	// Default Constructor
	BoidEngine();
	BoidEngine(vector< string > &sData);	// Overloaded Constructor for loading from file.
	~BoidEngine();

	// Public Functionality
	void draw( bool m_bPause );
	void update();

private:
	BoidEngine(const BoidEngine* pCopy);
	BoidEngine* operator=(const BoidEngine& pRHS);

	// Boid Structure
	struct Boid {
		vec3 vPos, vVelocity, vForce, vGravityForce;
		mat4 m4FrenetFrame;
	};

	// Private Variables
	float m_fScale;		// Scales the Boids
	float m_fDeltaT, m_fMinSpeed, m_fMaxSpeed;	 // Speed Limits
	vector< Boid > m_vBoids;		// Array of Boids
	vector< mat4 > m_vInstanceData;	// Array of Frenet frames for instance rendering
	vector< pair< unsigned int, vector< unsigned int > > > m_pScreenSpace;	// Boid Data Structure
	unsigned int m_iSpaceDim;		// Dimensions of the 3D Boid Grid
	unsigned int m_iSpaceDimSqr;	// Dimension squared of the 3D Boid Grid
	unsigned int m_iCurrTimeStep;	// Time Step tracker
	Mesh* m_pMesh;					// Boid Mesh
	Texture* m_pTexture;			// Boid Texture
	quat m_pQuaternion;				// Boid Quarternion for Orienting Boid Direction from Model
	unsigned int m_iNumBoids;		// Number of Boids in system
	float m_fAvoidRadius, m_fCohesRadius, m_fGathrRadius, m_fBoundaryRadius; // Radii for Boid Logic
	float m_fCohesWidth, m_fGathrWidth, m_fBoundaryLimit;
	vec3 m_fBoundaryCenter;			// Vector from origin to center of Boid-Zone

	// Private Functions
	void initializeBoids();
	void integrateBoid(unsigned int i);
	void checkBounds(Boid& pBoid);
	void compareBoids(Boid& pSource, Boid& pComp );
	unsigned int getSpaceIndex(const vec3& pPos);
	void checkCell(unsigned int iBoidIndex, unsigned int iCellIndex);
	vector< unsigned int >* getSpaceContents(unsigned int iIndex);
};

