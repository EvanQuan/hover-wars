#include "StaticEntity.h"

// Default Constructor
StaticEntity::StaticEntity(int iID, vec3 vPosition)
	: Entity( iID, vPosition )
{
	
}

// Destructor
StaticEntity::~StaticEntity()
{
	// Nothing to destruct
}