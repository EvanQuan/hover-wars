#pragma once
#include "stdafx.h"
#include "EntityHeaders/Entity.h"

// Name: SpatialDataMap
// Written By: James Coté
// Description: A Mapping Structure for Objects in the scene. Allows for sorting of objects for tailored rendering techniques
//  as well as pathfinding.
class SpatialDataMap
{
public:
	SpatialDataMap();
	~SpatialDataMap();

    // Initialization/Deconstruction of Data Map
    void initializeMap(float fLength, float fWidth, float fTileSize);
    void clearMap();

    // Draw Map for Debugging.
    void drawMap();

    // Checks for SpatialMap
    bool isInitialized() { return m_bIsInitialized; }

private:
    // Data for each Cell
    //  TODO: add different lists for different static objects (lights, pickups, etc.)
    struct sSpatialCell
    {
        vector< Entity* > pLocalEntities;
    };

    // Mapping of Cells in 2D array.
    vector< vector< sSpatialCell > > m_pSpatialMap;
    unsigned int m_iMaxX, m_iMaxY;
    float m_fTileSize;
    bool m_bIsInitialized;
    vec2 m_vOriginPos, m_vMaxDimensions;

    // Binds each Entity to their respective cell for quick lookup.
    unordered_map< int,                 // EntityID
                   pair<unsigned int,   // Indices for Cell Lookup
                        unsigned int>> m_pEntityMap;

    // Private Functions
    void generateVBOs();

    // data for debug rendering
    vector< vec3 > m_pVertices;
    vector< unsigned int > m_pIndices;
    GLuint m_iMapVertexArray, m_iMapVertexBuffer, m_iMapIndicesBuffer, m_iMapInstanceBuffer;
};

