#pragma once
#include "stdafx.h"
#include "EntityHeaders/StaticEntity.h"
#include "EntityHeaders/SpotLight.h"
#include "EntityHeaders/PointLight.h"

/************************\
 * Forward Declarations *
\************************/
class FlameTrail;

// Name: SpatialDataMap
// Written By: James Coté
// Description: A Mapping Structure for Objects in the scene. Allows for sorting of objects for tailored rendering techniques
//  as well as pathfinding.
class SpatialDataMap final
{
public:
    static SpatialDataMap* getInstance();
	virtual ~SpatialDataMap();

    // Initialization/Deconstruction of Data Map
    void initializeMap(float fLength, float fWidth, float fTileSize);
    void populateStaticMap(const unordered_map<int, unique_ptr<Entity>>* pMasterEntityList);
    void addDynamicEntity(const Entity* pNewDynamicEntity);
    void clearMap();
    void computePath(const vec2* pos1, const vec2* pos2);
    // Update Dynamic Entities
    void updateDynamicPosition(const Entity* pEntity, const vec3* pNewPos);

    // Draw Map for Debugging.
    void drawMap();

    // Checks for SpatialMap
    bool isInitialized() { return m_bIsInitialized; }
    vector<vec2> aStarSearch(vec2 player, vec2 dest);
    bool getMapIndices(const Entity* vEntity, unsigned int* iXMin, unsigned int* iXMax, unsigned int* iYMin, unsigned int* iYMax); // Returns the Map Indices from a given Entity.
    float getTileSize() const {return m_fTileSize;}
    glm::vec2 getWorldOffset() { return m_vOriginPos; }
    vector<uvec2> getShortestPath(uvec2 playerMin, uvec2 playerMax, uvec2 destMin, uvec2 destMax);
private:
    float evaluateDistance(const vec2* pos1, const vec2* pos2) const;
    static SpatialDataMap* m_pInstance;
    bool isValid(int x, int y) const;
    SpatialDataMap();                                           // Singleton Implementation
    SpatialDataMap(const SpatialDataMap* pCopy);                // Copy Constructor Overload
    SpatialDataMap& operator=(const SpatialDataMap* pCopy);     // Assignment Operator overload

    void initializeForDijkstras();
    void initializeForAStar(vector<vector<bool>> &closedList);
    // Static Data for each Cell
    struct sSpatialCell
    {
        vec2 vOriginPos;
        vector< const StaticEntity* > pLocalEntities;
        vector< const SpotLight* > pLocalSpotLights;
        vector< const PointLight* > pLocalPointLights;
        vector< const FlameTrail* > pLocalInteractableEntities;
        unsigned int iStaticSize;
        int parentX = -1;
        int parentY = -1;
        double fCost, gCost, hCost;
        int x, y;
        bool visited = false;
    };


    double calculateH(int x, int y, sSpatialCell dest) const;
    vector<vec2> makePath(sSpatialCell dest);
    // Mapping of Cells in 2D array.
    vector< vector< sSpatialCell > > m_pSpatialMap;
    unsigned int m_iMaxX, m_iMaxY;
    float m_fTileSize;
    bool m_bIsInitialized;
    vec2 m_vOriginPos, m_vMaxDimensions;

    // Binds each Entity to their respective cell for quick lookup.
    unordered_map< int,                 // EntityID
                   vector<pair<unsigned int,   // Indices for Cell Lookup
                               unsigned int>> > m_pEntityMap;

    // Private Functions
    void generateGridVBOs();
    void addEntity(const Entity* vEntity, unsigned int iXMin, unsigned int iXMax, unsigned int iYMin, unsigned int iYMax); // Add The Entity to the Spatial Map as well as the EntityMap.
    void getVectToPos(const vec3* vWorldPosition, vec2* vToPos);
    void computeNewDynamicPosition(const Entity* vEntity, const vec3* vNewPos);
    void addSquareIndices(vector<unsigned int>* pIndicesBuffer, unsigned int iXIndex, unsigned int iYIndex);
    bool getNearestCar(int currID, vector<int> IDs, vec2 &minPos);
#ifdef _DEBUG
    // data for debug rendering
    vector< vec3 > m_pVertices;
    vector< unsigned int > m_pGridIndices, m_pPopulatedIndices;

    // Structure for Drawing each Dynamic Entity
    struct sDynamicDrawInfo
    {
        vector< unsigned int > pDynamicIndices;
        GLuint iDynamicIBO;
    };

    unordered_map< int/*EntityID*/, sDynamicDrawInfo/*DrawInfo*/ > m_pDynamicIndicesMap;
    vector< pair<unsigned int, unsigned int>> m_pPopulatedSquareReference;
    GLuint m_iMapVertexArray, m_iMapVertexBuffer, m_iMapIndicesBuffer, m_iPopulatedIndicesBuffer, m_iMapInstanceBuffer;
#endif
};
