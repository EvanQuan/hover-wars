#include "SpatialDataMap.h"
#include "ShaderManager.h"
#include "EntityHeaders/FlameTrail.h"
#include <queue>
#include <stack>
/*************\
 * Constants *
\*************/
const float ALPHA           = 0.5f;
const vec4 GRID_COLOR       = vec4(1.0f, 0.1568627450980392f, 0.0f, ALPHA);                                                 // Ferrari Red
const vec4 POP_COLOR        = vec4(0.4941176470588235f, 0.9764705882352941f, 1.0f, ALPHA);                                  // Electric Blue
const vec4 SPOT_COLOR       = vec4(0.3764705882352941f, 0.1843137254901961f, 0.4196078431372549f, ALPHA);                   // Deep Purple
const vec4 POINT_COLOR      = vec4(0.9411764705882353f, 1.0f, 0.9411764705882353f, ALPHA);                                  // Azure White
const vec4 DYNAMIC_COLOR    = vec4(0.0f, 0.65882352941176470588235294117647f, 0.41960784313725490196078431372549f, ALPHA);    // Jade
float OVERLAY_HEIGHT = 0.1f;

/***********\
 * Defines *
\***********/
#define MIN_INDEX 0
#define MAX_INDEX 1

/****************************\
 * Singleton Implementation *
\****************************/
SpatialDataMap* SpatialDataMap::m_pInstance = nullptr;

SpatialDataMap* SpatialDataMap::getInstance()
{
    if (nullptr == m_pInstance)
        m_pInstance = new SpatialDataMap();
    return m_pInstance;
}

// Default Constructor for Data Map
SpatialDataMap::SpatialDataMap()
{
    m_bIsInitialized = false;
}

// Destructor for Data Map
SpatialDataMap::~SpatialDataMap()
{
    // Reset Map
    clearMap();
}

// Clears the Map to a fresh state for reinitialization
void SpatialDataMap::clearMap()
{
    // Clear the unordered Map
    m_pEntityMap.clear();

    // Iterate through each row in the spatial Map and clear the elements.
    for (vector<vector<sSpatialCell>>::iterator iter = m_pSpatialMap.begin();
        iter != m_pSpatialMap.end();
        ++iter)
        iter->clear();

    // clear the base vector of its elements
    m_pSpatialMap.clear();

#ifdef _DEBUG
    // Delete VBOs and VAOs
    glDeleteBuffers(1, &m_iMapIndicesBuffer);
    glDeleteBuffers(1, &m_iMapVertexBuffer);
    glDeleteBuffers(1, &m_iMapInstanceBuffer);
    glDeleteBuffers(1, &m_iPopulatedIndicesBuffer);

    // Clear Dynamic IBOs
    for (unordered_map<int, sDynamicDrawInfo>::iterator iter = m_pDynamicIndicesMap.begin();
        iter != m_pDynamicIndicesMap.end();
        ++iter)
        glDeleteBuffers(1, &(iter->second.iDynamicIBO));

    glDeleteVertexArrays(1, &m_iMapVertexArray);

    // Clear the Dynamic Indices Map.
    m_pDynamicIndicesMap.clear();
#endif

    m_bIsInitialized = false;
}

// Initialize the Map dimensions 
void SpatialDataMap::initializeMap(float fLength, float fWidth, float fTileSize)
{
    // Local Variables
    float fHalfLength = fLength * 0.5f;
    float fHalfWidth = fWidth * 0.5f;
    m_fTileSize = fTileSize;
    m_iMaxX = static_cast<unsigned int>(floor(fWidth / fTileSize));
    m_iMaxY = static_cast<unsigned int>(floor(fLength / fTileSize));

    // Set the Origin position for reference wrt world coordinates as well as the maximum limit of the map.
    m_vOriginPos = vec2(-fHalfWidth, -fHalfLength);
    m_vMaxDimensions = vec2(fWidth, fLength) + m_vOriginPos;

    // Initialize Map Arrays
    m_pSpatialMap.resize(m_iMaxX, vector<sSpatialCell>(m_iMaxY));
    vec2 vOriginOffset = m_vOriginPos;

    // Set the relative Origin Position for each cell.
    for (unsigned int x = 0; x < m_iMaxX; ++x)
    {
        // Go through each column
        for (unsigned int y = 0; y < m_iMaxY; ++y)
        {
            // Set the Relative Position and update to the next column position.
            m_pSpatialMap[x][y].vOriginPos = vOriginOffset;
            m_pSpatialMap[x][y].iStaticSize = 0;
            vOriginOffset.y += m_fTileSize;
        }

        // Reset the Offset Counter for the loop.
        vOriginOffset.y = m_vOriginPos.y;
        vOriginOffset.x += m_fTileSize;
    }

    // Generate the VBOs for drawing the map.
#ifdef _DEBUG
    generateGridVBOs();
#endif

    // Set the Initialized Flag.
    m_bIsInitialized = true;
}

// Populate Spatial Data Map with Entities in the scene.
void SpatialDataMap::populateStaticMap(const unordered_map<int, unique_ptr<Entity>>* pMasterEntityList)
{
    // Local Variables
    unsigned int iXMin, iXMax, iYMin, iYMax; // Indices for determining Entity Position.

    for (unordered_map<int, unique_ptr<Entity>>::const_iterator iter = pMasterEntityList->begin();
        iter != pMasterEntityList->end();
        ++iter)
    {
        // Add Entity to Spatial Map
        if (getMapIndices(iter->second.get(), &iXMin, &iXMax, &iYMin, &iYMax)) // Verify the Indices received are valid.
        {
            addEntity(iter->second.get(), iXMin, iXMax, iYMin, iYMax);
        }
    }

#ifdef _DEBUG // Only deal with GPU in Debug release
    // Add Populated Indices list to the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iPopulatedIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pPopulatedIndices.size() * sizeof(unsigned int), m_pPopulatedIndices.data(), GL_STATIC_DRAW);
#endif
}

void SpatialDataMap::addDynamicEntity(const Entity* pNewDynamicEntity)
{
    // Local Variables
    unsigned int iXMin, iXMax, iYMin, iYMax; // Indices for determining Entity Position.

    // Add Entity to Spatial Map
    if (getMapIndices(pNewDynamicEntity, &iXMin, &iXMax, &iYMin, &iYMax)) // Verify the Indices received are valid.
    {
        addEntity(pNewDynamicEntity, iXMin, iXMax, iYMin, iYMax);
    }
}

// Add The Entity to the Spatial Map as well as the EntityMap.
void SpatialDataMap::addEntity(const Entity* vEntity, unsigned int iXMin, unsigned int iXMax, unsigned int iYMin, unsigned int iYMax)
{
    // Local Variables
    bool bValidEntity = true;

    if (m_pEntityMap.find(vEntity->getID()) == m_pEntityMap.end())  // Add an entry to the Entity Map if one doesn't exist already.
    {
        m_pEntityMap.insert(make_pair(vEntity->getID(), vector<pair<unsigned int, unsigned int>>()));
        m_pEntityMap[vEntity->getID()].push_back(make_pair(iXMin, iYMin));
        m_pEntityMap[vEntity->getID()].push_back(make_pair(iXMax, iYMax));
    }

    // Evaluate all the indices that the Entity inhabits.
    for (unsigned int x = iXMin; x <= iXMax; ++x)
        for (unsigned int y = iYMin; y <= iYMax; ++y)
        {
            switch (vEntity->getType())
            {
            case ENTITY_INTERACTABLE:
                m_pSpatialMap[x][y].pLocalInteractableEntities.push_back(static_cast<const FlameTrail*>(vEntity));          // Push the Interactable Entity into the spatial map.
                break;
            case ENTITY_POINT_LIGHT:
                m_pSpatialMap[x][y].pLocalPointLights.push_back(static_cast<const PointLight*>(vEntity));                   // Push the Static Entity into the spatial map.
                break;
            case ENTITY_SPOT_LIGHT:
                m_pSpatialMap[x][y].pLocalSpotLights.push_back(static_cast<const SpotLight*>(vEntity));                     // Push the Static Entity into the spatial map.
                break;
            case ENTITY_STATIC:
                m_pSpatialMap[x][y].pLocalEntities.push_back(static_cast<const StaticEntity*>(vEntity));                    // Push the Static Entity into the spatial map.
                break;
#ifdef _DEBUG
            case ENTITY_HOVERCRAFT:
                // Add this entry to the Dynamic Indices Map.
                if (m_pDynamicIndicesMap.find(vEntity->getID()) == m_pDynamicIndicesMap.end())
                    m_pDynamicIndicesMap.insert(make_pair(vEntity->getID(), sDynamicDrawInfo()));

                // Add Indices for this cell.
                addSquareIndices(&m_pDynamicIndicesMap[vEntity->getID()].pDynamicIndices, x, y);
#endif
            default: // Waterfall Dynamic Entities to not be Valid Entities for Static Map.
                bValidEntity = false;
                break;
            }


            // If the Entity is a Valid Entity Type, add it to the EntityMap
            if (bValidEntity)
            {
                m_pSpatialMap[x][y].iStaticSize++;                            // Track Static size of the square.
#ifdef _DEBUG
                // Add Indices for a populated square if this is the first Entity added to the list.
                //  This is solely for debug drawing.
                if (1 == m_pSpatialMap[x][y].iStaticSize)
                {
                    // Add Indices for Drawing.
                    addSquareIndices(&m_pPopulatedIndices, x, y);

                    // Store reference for the square to determine color.
                    m_pPopulatedSquareReference.push_back(make_pair(x, y));
                }
#endif // _DEBUG
            }

            // Reset boolean for next loop
            bValidEntity = true;

        }

#ifdef _DEBUG
    // Generate IBO if the Entity was a PLAYER_ENTITY
    if (ENTITY_HOVERCRAFT == vEntity->getType() )
        m_pDynamicIndicesMap[vEntity->getID()].iDynamicIBO =
        SHADER_MANAGER->genIndicesBuffer(m_iMapVertexArray, m_pDynamicIndicesMap[vEntity->getID()].pDynamicIndices.data(),
                                         m_pDynamicIndicesMap[vEntity->getID()].pDynamicIndices.size() * sizeof(unsigned int), GL_DYNAMIC_DRAW);
#endif
}

// Function to Update new Dynamic Position for a given Dynamic Entity
void SpatialDataMap::updateDynamicPosition(const Entity* pEntity, const vec3* pNewPos)
{
    // Local Variables
    unsigned int iOldXMin, iOldXMax, iOldYMin, iOldYMax;
    vec2 vToNewNegPos, vToNewPosPos;
    vec3 vNewNegPos, vNewPosPos;
    vec3 vNegativeOffset, vPositiveOffset;
    bool bChange = false;

    // Get the Range of the Entity Position
    iOldXMin = m_pEntityMap[pEntity->getID()][MIN_INDEX].first;
    iOldXMax = m_pEntityMap[pEntity->getID()][MAX_INDEX].first;
    iOldYMin = m_pEntityMap[pEntity->getID()][MIN_INDEX].second;
    iOldYMax = m_pEntityMap[pEntity->getID()][MAX_INDEX].second;
    
    // Get Dimensions for the Entity
    pEntity->getSpatialDimensions(&vNegativeOffset, &vPositiveOffset);

    // Calculate World Dimensions for Entity
    vNewNegPos = *pNewPos + vNegativeOffset;
    vNewPosPos = *pNewPos + vPositiveOffset;

    // Get Vectors
    getVectToPos(&vNewNegPos, &vToNewNegPos);
    getVectToPos(&vNewPosPos, &vToNewPosPos);

    // Determine if a movement happened.
    // clever -Austin
    bChange |= iOldXMin != static_cast<unsigned int>(floor(vNewNegPos.x / m_fTileSize));
    bChange |= iOldYMin != static_cast<unsigned int>(floor(vNewNegPos.y / m_fTileSize));
    bChange |= iOldXMax != static_cast<unsigned int>(floor(vNewPosPos.x / m_fTileSize));
    bChange |= iOldYMax != static_cast<unsigned int>(floor(vNewPosPos.y / m_fTileSize));

    if (bChange)
        computeNewDynamicPosition(pEntity, pNewPos);
}
float SpatialDataMap::evaluateDistance(const vec2* pos1, const vec2* pos2) {
    return abs(pos1->x - pos2->x) + abs(pos1->y - pos2->y);
}
bool SpatialDataMap::isValid(int x,int y) {
    return x < (int)m_pSpatialMap.size()  && x >= 0 && y < (int)m_pSpatialMap[x].size() && y >= 0 && (int)m_pSpatialMap[x][y].pLocalEntities.size() == 0;
}
double SpatialDataMap::calculateH(int x, int y, sSpatialCell dest) {
    double H = (sqrt((x - dest.x)*(x - dest.x)
        + (y - dest.y)*(y - dest.y)));
    return H;
}

vector<vec2> SpatialDataMap::modifiedDikjistras(vec2 playerMin, vec2 playerMax, vec2 destMin, vec2 destMax) {
    vec2 dest = destMin;
    if (!isValid((int)dest.x, (int)dest.y)) {
        int yMul = (destMax.y - destMin.y) == 0 ? 0 : (destMax.y - destMin.y) / abs(destMax.y - destMin.y);
        int xMul = (destMax.x - destMin.x) == 0 ? 0 : (destMax.x - destMin.x) / abs(destMax.x - destMin.x);
        int xLoop = destMin.x - xMul;
        bool breakLoop = false;
        do {
            xLoop += xMul;
            int yLoop = destMin.y - yMul;
            do {
                yLoop += yMul;
                if (isValid(xLoop, yLoop))
                {
                    dest = vec2(xLoop, yLoop);
                    breakLoop = true;
                    break;
                }
            } while (yLoop != destMax.y);

            if (breakLoop) break;
        } while (xLoop != destMax.x);
        for (int x = -1; x < 2 && !breakLoop; x++) {
            for (int y = -1; y < 2; y++) {
                if (isValid((int)dest.x + x, (int)dest.y + y)) {
                    dest.x += x;
                    dest.y += y;
                    breakLoop = true;
                    break;
                }
            }
        }
    }
    vec2 player = playerMin;
    if (!isValid((int)player.x, (int)player.y)) {
        int yMul = (playerMax.y - playerMin.y) == 0 ? 0 : (playerMax.y - playerMin.y) / abs(playerMax.y - playerMin.y);
        int xMul = (playerMax.x - playerMin.x) == 0 ? 0 : (playerMax.x - playerMin.x) / abs(playerMax.x - playerMin.x);
        int xLoop = playerMin.x - xMul;
        bool breakLoop = false;
        do {
            xLoop += xMul;
            int yLoop = playerMin.y - yMul;
            do {
                yLoop += yMul;
                if (isValid(xLoop, yLoop))
                {
                    player = vec2(xLoop, yLoop);
                    breakLoop = true;
                    break;
                }
            } while (yLoop != playerMax.y);

            if (breakLoop) break;
        } while (xLoop != playerMax.x);
        for (int x = -1; x < 2 && !breakLoop; x++) {
            for (int y = -1; y < 2; y++) {
                if (isValid((int)player.x + x, (int)player.y + y)) {
                    player.x += x;
                    player.y += y;
                    breakLoop = true;
                    break;//just to spite paul
                }
            }
        }
    }
    for (unsigned int i = 0; i < m_pSpatialMap.size(); i++) {
        for (unsigned int j = 0; j < m_pSpatialMap[0].size(); j++) {
            m_pSpatialMap[i][j].visited = false;
            m_pSpatialMap[i][j].parentX = -1;
            m_pSpatialMap[i][j].parentY = -1;
        }
    }
    queue<vec2> valuesToTry;
    valuesToTry.push(player);
    vec2 currPos;
    bool exitLoop = false;
    while (valuesToTry.size() > 0) {
        currPos = valuesToTry.front();
        valuesToTry.pop();
        for (int x = -1; x < 2; x++) {
            if (x == 0) {
                for (int y = -1; y < 2; y++) {
                    if ((int)currPos.x + x == dest.x && (int)currPos.y + y == dest.y) {
                        m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].visited = true;
                        m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].parentX = (int)currPos.x;
                        m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].parentY = (int)currPos.y;
                        currPos.x += x;
                        currPos.y += y;
                        exitLoop = true;
                        break;
                    }
                    if (isValid((int)currPos.x + x, (int)currPos.y + y) && !m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].visited) {
                        m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].visited = true;
                        m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].parentX = (int)currPos.x;
                        m_pSpatialMap[(int)currPos.x + x][(int)currPos.y + y].parentY = (int)currPos.y;
                        valuesToTry.push(vec2((int)currPos.x + x, (int)currPos.y + y));
                    }
                }
            }
            else {
                if ((int)currPos.x + x == dest.x && (int)currPos.y == dest.y) {
                    m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].visited = true;
                    m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].parentX = (int)currPos.x;
                    m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].parentY = (int)currPos.y;
                    currPos.x += x;
                    exitLoop = true;
                    break;
                }
                if (isValid((int)currPos.x + x, (int)currPos.y) && !m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].visited) {
                    m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].visited = true;
                    m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].parentX = (int)currPos.x;
                    m_pSpatialMap[(int)currPos.x + x][(int)currPos.y].parentY = (int)currPos.y;
                    valuesToTry.push(vec2((int)currPos.x + x, (int)currPos.y));
                }
            }
            if (exitLoop) break;
        }
        if (exitLoop) break;
    }
    vector<vec2> returnPath;
    while (exitLoop && !(currPos.x == player.x && currPos.y == player.y)) {     // @AustinEaton : currPos potentially not initialized
        returnPath.push_back(vec2(m_pSpatialMap[(int)currPos.x][(int)currPos.y].parentX,
            m_pSpatialMap[(int)currPos.x][(int)currPos.y].parentY));
        int temp = (int)currPos.x;
        if (m_pSpatialMap[temp][(int)currPos.y].parentX == -1 ||
            -1 == m_pSpatialMap[temp][(int)currPos.y].parentY) {
            break;
        }
        currPos.x = (float)m_pSpatialMap[temp][(int)currPos.y].parentX;
        currPos.y = (float)m_pSpatialMap[temp][(int)currPos.y].parentY;
    }
    return returnPath;
}
vector<vec2> emptyVectorArray; // bad practice but saves us having to reinstacate every frame
vector<vec2> SpatialDataMap::makePath(sSpatialCell dest) {
    try {
        int x = dest.x;
        int y = dest.y;
        stack<vec2> path;
        vector<vec2> usablePath;

        while (!(m_pSpatialMap[x][y].parentX == x && m_pSpatialMap[x][y].parentY == y)
            && m_pSpatialMap[x][y].x != -1 && m_pSpatialMap[x][y].y != -1)
        {
            path.push(vec2(m_pSpatialMap[x][y].x, m_pSpatialMap[x][y].y));
            int tempX = m_pSpatialMap[x][y].parentX;
            int tempY = m_pSpatialMap[x][y].parentY;
            x = tempX;
            y = tempY;

        }
        path.push(vec2(m_pSpatialMap[x][y].x, m_pSpatialMap[x][y].y));

        while (!path.empty()) {
            vec2 top = path.top();
            path.pop();
            usablePath.emplace_back(top);
        }
        return usablePath;
    }
    catch (const exception& e) {
        cout << e.what() << endl;
    }
    return emptyVectorArray;
}
// A Function to find the shortest path between 
// a given source cell to a destination cell according 
// to A* Search Algorithm

vector<vec2> SpatialDataMap::aStarSearch(vec2 player, vec2 dest) {
    if (!isValid((int) dest.x, (int)dest.y)) {
        bool breakLoop = false;
        for (int x = -1; x < 2;x++) {
            for (int y = -1; y < 2; y++) {
                if (isValid((int)dest.x + x, (int)dest.y + y)) {
                    dest.x += x;
                    dest.y += y;
                    breakLoop = true;
                    break;
                }
                if (breakLoop) break;
            }
        }
    }
    if (!isValid((int)player.x, (int)player.y)) {
        bool breakLoop = false;
        for (int x = -1; x < 2; x++) {
            for (int y = -1; y < 2; y++) {
                if (isValid((int)player.x + x, (int)player.y + y)) {
                    player.x += x;
                    player.y += y;
                    breakLoop = true;
                    break;
                }
                if (breakLoop) break;
            }
        }
    }
    if (player.x == dest.x && player.y == dest.y) {
        return emptyVectorArray;
        //You clicked on yourself
    }
    vector<vector<bool>> closedList;
    for (unsigned int i = 0; i < m_pSpatialMap.size(); i++) {
        vector<bool> type;
        for (unsigned int j = 0; j < m_pSpatialMap[0].size(); j++) {
            type.push_back(false);
        }
        closedList.push_back(type);
    }
    //Initialize whole map
    //Node allMap[50][25];
    for (unsigned int x = 0; x < m_pSpatialMap.size(); x++) {
        vector<bool> type;
        for (unsigned int y = 0; y < m_pSpatialMap[0].size(); y++) {
            m_pSpatialMap[x][y].fCost = FLT_MAX;
            m_pSpatialMap[x][y].gCost = FLT_MAX;
            m_pSpatialMap[x][y].hCost = FLT_MAX;
            m_pSpatialMap[x][y].parentX = -1;
            m_pSpatialMap[x][y].parentY = -1;
            m_pSpatialMap[x][y].x = x;
            m_pSpatialMap[x][y].y = y;
            type.push_back(false);

        }
        closedList.push_back(type);
    }

    //Initialize our starting list
    int x = (int)player.x;
    int y = (int)player.y;
    m_pSpatialMap[x][y].fCost = 0.0;
    m_pSpatialMap[x][y].gCost = 0.0;
    m_pSpatialMap[x][y].hCost = 0.0;
    m_pSpatialMap[x][y].parentX = x;
    m_pSpatialMap[x][y].parentY = y;

    vector<sSpatialCell> openList;
    openList.emplace_back(m_pSpatialMap[x][y]);
    bool destinationFound = false;
    while (!openList.empty()) {
        sSpatialCell node;
            do {
                //This do-while loop could be replaced with extracting the first
                //element from a set, but you'd have to make the openList a set.
                //To be completely honest, I don't remember the reason why I do
                //it with a vector, but for now it's still an option, although
                //not as good as a set performance wise.
                double temp = FLT_MAX;
                bool hasBeenSet = false;
                vector<sSpatialCell>::iterator itNode;
                for (vector<sSpatialCell>::iterator it = openList.begin();
                    it != openList.end(); it = next(it)) {
                    sSpatialCell n = *it;
                    if (n.fCost < temp) {
                        temp = n.fCost;
                        hasBeenSet = true;
                        itNode = it;
                    }
                }
                if (!hasBeenSet) {
                    return emptyVectorArray;
                }
                node = *itNode;
                openList.erase(itNode);
            } while (isValid(node.x, node.y) == false);
        
        x = node.x;
        y = node.y;
        closedList[x][y] = true;

        //For each neighbour starting from North-West to South-East
        for (int newX = -1; newX <= 1; newX++) {
            for (int newY = -1; newY <= 1; newY++) {
                double gNew, hNew, fNew;
                if (isValid(x + newX, y + newY)) {
                    if (player.x + newX == dest.x && player.y + newY == dest.y)
                    {
                        //Destination found - make path
                        m_pSpatialMap[x + newX][y + newY].parentX = x;
                        m_pSpatialMap[x + newX][y + newY].parentY = y;
                        destinationFound = true;
                        return makePath(m_pSpatialMap[(int)dest.x][(int)dest.y]);
                    }
                    else if (closedList[x + newX][y + newY] == false)
                    {
                        gNew = node.gCost + 1.0;
                        hNew = calculateH(x + newX, y + newY, m_pSpatialMap[(int)dest.x][(int)dest.y]);
                        fNew = gNew + hNew;
                        // Check if this path is better than the one already present
                        if (m_pSpatialMap[x + newX][y + newY].fCost == FLT_MAX ||
                            m_pSpatialMap[x + newX][y + newY].fCost > fNew)
                        {
                            // Update the details of this neighbour node
                            m_pSpatialMap[x + newX][y + newY].fCost = fNew;
                            m_pSpatialMap[x + newX][y + newY].gCost = gNew;
                            m_pSpatialMap[x + newX][y + newY].hCost = hNew;
                            m_pSpatialMap[x + newX][y + newY].parentX = x;
                            m_pSpatialMap[x + newX][y + newY].parentY = y;
                            openList.emplace_back(m_pSpatialMap[x + newX][y + newY]);
                        }
                    }
                }
            }
        }
    }
    return emptyVectorArray;
}
bool SpatialDataMap::getNearestCar(int currID,vector<int> IDs, vec2 &minPos) {
    vec2 mPos(0, 0);
    for (int id : IDs) {
        if (currID == id) {
            pair<unsigned int, unsigned int> loc = m_pEntityMap[currID][0];
            mPos.x = (float)loc.first;
            mPos.y = (float)loc.second;
        }
    }
    float minValue = 100000;
    for (int id : IDs) {
        if (currID != id) {
            pair<unsigned int, unsigned int> loc = m_pEntityMap[currID][0];
            vec2 currPos(0, 0);
            currPos.x = (float) loc.first;
            currPos.y = (float) loc.second;
            if (float currValue = evaluateDistance(&mPos,&currPos) < minValue) {
                minPos = currPos;
                minValue = currValue;
            }
        }
    }
    return minValue == 100000;
}
void SpatialDataMap::computeNewDynamicPosition(const Entity* pEntity, const vec3* pNewPos)
{
    // Local Variables
    unsigned int iXMin, iXMax, iYMin, iYMax;

    getMapIndices(pEntity, &iXMin, &iXMax, &iYMin, &iYMax);

    // Add new bounds for the Entity in the Entity Map
    m_pEntityMap[pEntity->getID()][MIN_INDEX] = make_pair(iXMin, iYMin);
    m_pEntityMap[pEntity->getID()][MAX_INDEX] = make_pair(iXMax, iYMax);

#ifdef _DEBUG
    // Clear current Indices List
    m_pDynamicIndicesMap[pEntity->getID()].pDynamicIndices.clear();

    // Compute new Indices
    for (unsigned int x = iXMin; x <= iXMax; ++x)
        for (unsigned int y = iYMin; y <= iYMax; ++y)
        {
            // Add Indices for Drawing the entity.
            addSquareIndices(&m_pDynamicIndicesMap[pEntity->getID()].pDynamicIndices, x, y);
        }

    // Add New Indices list to the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pDynamicIndicesMap[pEntity->getID()].iDynamicIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pDynamicIndicesMap[pEntity->getID()].pDynamicIndices.size() * sizeof(unsigned int), m_pDynamicIndicesMap[pEntity->getID()].pDynamicIndices.data(), GL_DYNAMIC_DRAW);
#endif // _DEBUG :> for drawing
}

// Draw the Data Map for Debugging
void SpatialDataMap::drawMap()
{
#ifdef _DEBUG
    if (m_bIsInitialized)
    {
        // Bind Vertex Array and set Program
        glBindVertexArray(m_iMapVertexArray);
        glUseProgram(SHADER_MANAGER->getProgram(ShaderManager::eShaderType::DEBUG_SHDR));

        // Set the color for the Spacial Map Outline
        SHADER_MANAGER->setUniformVec4(ShaderManager::eShaderType::DEBUG_SHDR, "vColor", &GRID_COLOR);

        // Draw the Map
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iMapIndicesBuffer);
        glDrawElementsInstanced(GL_LINES, m_pGridIndices.size(), GL_UNSIGNED_INT, 0, 1);

        // Draw the Dynamic Squares
        unsigned int i = 0;
        vec4 vColor = DYNAMIC_COLOR;
        // Iterate through all Dynamic Entities' Indices
        for (unordered_map<int, sDynamicDrawInfo>::iterator iter = m_pDynamicIndicesMap.begin();
            iter != m_pDynamicIndicesMap.end();
            ++iter)
        {   // Iterate through all covered cells for the entity in the entity map.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iter->second.iDynamicIBO);
            for (unsigned int x = m_pEntityMap[iter->first][MIN_INDEX].first; x <= m_pEntityMap[iter->first][MAX_INDEX].first; ++x)
                for (unsigned int y = m_pEntityMap[iter->first][MIN_INDEX].second; y <= m_pEntityMap[iter->first][MAX_INDEX].second; ++y)
                {
                    if (!m_pSpatialMap[x][y].pLocalEntities.empty())
                        vColor = (vColor * 0.5f) + (GRID_COLOR * 0.5f);
                    else if (!m_pSpatialMap[x][y].pLocalPointLights.empty())
                        vColor = (vColor * 0.5f) + (POINT_COLOR * 0.5f);
                    else if (!m_pSpatialMap[x][y].pLocalSpotLights.empty())
                        vColor = (vColor * 0.5f) + (SPOT_COLOR * 0.5f);

                    // Set the blended color.
                    SHADER_MANAGER->setUniformVec4(ShaderManager::eShaderType::DEBUG_SHDR, "vColor", &vColor);
                    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)((i << 2) * sizeof(unsigned int)), 1);
                    ++i;    // Increment to next set of indices in the GPU
                    vColor = DYNAMIC_COLOR;
                }

            // Reset the i Index
            i = 0;
        }

        // Draw the Populated Squares
        vColor = vec4(1.0f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iPopulatedIndicesBuffer);
        for (unsigned int j = 0; j < m_pPopulatedSquareReference.size(); ++j)
        {
            if (!m_pSpatialMap[m_pPopulatedSquareReference[j].first][m_pPopulatedSquareReference[j].second].pLocalEntities.empty())
                vColor = (vColor * 0.5f) + (GRID_COLOR * 0.5f);
            else if (!m_pSpatialMap[m_pPopulatedSquareReference[j].first][m_pPopulatedSquareReference[j].second].pLocalPointLights.empty())
                vColor = (vColor * 0.5f) + (POINT_COLOR * 0.5f);
            else if (!m_pSpatialMap[m_pPopulatedSquareReference[j].first][m_pPopulatedSquareReference[j].second].pLocalSpotLights.empty())
                vColor = (vColor * 0.5f) + (SPOT_COLOR * 0.5f);

            SHADER_MANAGER->setUniformVec4(ShaderManager::eShaderType::DEBUG_SHDR, "vColor", &vColor);
            glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)((j << 2) * sizeof(unsigned int)), 1);
            vColor = vec4(1.0f);
        }
    }
#endif // _DEBUG
}

/*********************************************************************************\
* Private Functions                                                              *
\*********************************************************************************/

// Returns the Map Indices from a given Position.
bool SpatialDataMap::getMapIndices(const Entity* vEntity, unsigned int* iXMin, unsigned int* iXMax, unsigned int* iYMin, unsigned int* iYMax)
{
    // Get Spatial Dimensions from the Entity.
    vec3 vNegativeOffset(0.0f), vPositiveOffset(0.0f), vPosition = vEntity->getPosition();
    vEntity->getSpatialDimensions(&vNegativeOffset, &vPositiveOffset);
    bool bReturnValue = false;

    if (vNegativeOffset != vec3(0.0f) || vPositiveOffset != vec3(0.0f))
    {
        // Move Offsets to World Coordinates
        vNegativeOffset = vPosition + vNegativeOffset;
        vPositiveOffset = vPosition + vPositiveOffset;

        // Get Vectors wrt SpatialMap Origin
        vec2 vVectorToNegPosition, vVectorToPosPosition;
        getVectToPos(&vNegativeOffset, &vVectorToNegPosition);
        getVectToPos(&vPositiveOffset, &vVectorToPosPosition);

        // Compute Indices range.
        *iXMin = static_cast<unsigned int>(floor(vVectorToNegPosition.x / m_fTileSize));
        *iXMax = std::min(static_cast<unsigned int>(floor(vVectorToPosPosition.x / m_fTileSize)), m_iMaxX - 1);
        *iYMin = static_cast<unsigned int>(floor(vVectorToNegPosition.y / m_fTileSize));
        *iYMax = std::min(static_cast<unsigned int>(floor(vVectorToPosPosition.y / m_fTileSize)), m_iMaxY - 1);

        // Verify that data is valid.
        bReturnValue = true;
    }

    return bReturnValue; // Tell the caller whether the data is valid or not (Is it out of range?)
}

// Generates the VBOs for the Grid outline in debug mode.
void SpatialDataMap::generateGridVBOs()
{
#ifdef _DEBUG
    // Local Variables
    unsigned int iCurrXIndex, iCurrIndex;

    // Pre-compute sizes of the arrays for Vertices and Indices
    m_pVertices.reserve((m_iMaxX + 1) * (m_iMaxY + 1));
    m_pGridIndices.reserve((m_iMaxX * (m_iMaxY + 1)) * 4);

    // Iterate through all possible vertices and populate the vertices and indices for rendering.
    for (unsigned int x = 0; x <= m_iMaxX; ++x)
    {
        iCurrXIndex = x * (m_iMaxX + 1);    // Set current Row Index;

        // Evaluate Row
        for (unsigned int y = 0; y <= m_iMaxY; ++y)
        {
            // Push back Vertex for this point.
            m_pVertices.push_back(vec3(m_vOriginPos.x + (x * m_fTileSize),
                OVERLAY_HEIGHT,
                m_vOriginPos.y + (y * m_fTileSize)));

            // Store Current Index
            iCurrIndex = iCurrXIndex + y;

            // Line from current row to next row.
            if (m_iMaxX != x)
            {
                m_pGridIndices.push_back(iCurrIndex);
                m_pGridIndices.push_back(m_pGridIndices.back() + m_iMaxX + 1);
            }

            // Line along this row.
            if (m_iMaxY != y)
            {
                m_pGridIndices.push_back(iCurrIndex);
                m_pGridIndices.push_back(m_pGridIndices.back() + 1);
            }
        }
    }

    // Generate Vertex Array
    glGenVertexArrays(1, &m_iMapVertexArray);

    // Generate Vertex Buffer
    m_iMapVertexBuffer = SHADER_MANAGER->genVertexBuffer(m_iMapVertexArray, m_pVertices.data(), m_pVertices.size() * sizeof(vec3), GL_STATIC_DRAW);
    SHADER_MANAGER->setAttrib(m_iMapVertexArray, 0, 3, sizeof(vec3), (void*)0);

    // Generate Indices Buffer
    m_iMapIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(m_iMapVertexArray, m_pGridIndices.data(), m_pGridIndices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
    m_iPopulatedIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(m_iMapVertexArray, (void*)0, 0, GL_STATIC_DRAW);

    // Generate Instance Buffer for Rendering in Debug Shader
    mat4 m4TransformationMatrix = mat4(1.0f);
    m_iMapInstanceBuffer = SHADER_MANAGER->genInstanceBuffer(m_iMapVertexArray, 1, &m4TransformationMatrix, sizeof(mat4), GL_STATIC_DRAW);
#endif // _DEBUG
}

/**************************************************************************************************\
 * Helper Functions                                                                               *
\**************************************************************************************************/

// Finds and returns a vector from the Map origin to the World Position
void SpatialDataMap::getVectToPos(const vec3* vWorldPosition, vec2* vToPos)
{
    *vToPos = vec2(std::max(vWorldPosition->x - m_vOriginPos.x, 0.0f), std::max(vWorldPosition->z - m_vOriginPos.y, 0.0f));
}

// Computes the Indices for a given Cell Index
void SpatialDataMap::addSquareIndices(vector<unsigned int>* pIndicesBuffer, unsigned int iXIndex, unsigned int iYIndex)
{
    // Add Indices for coloring the populated square
    /*
        1--2
        | /|
        |/ |
        3--4
    */
    iXIndex = iXIndex * (m_iMaxX + 1);
    pIndicesBuffer->push_back(iXIndex + iYIndex);                // 1
    pIndicesBuffer->push_back(iXIndex + iYIndex + 1);            // 2
    pIndicesBuffer->push_back(iXIndex + m_iMaxX + iYIndex + 1);  // 3
    pIndicesBuffer->push_back(iXIndex + m_iMaxX + iYIndex + 2);  // 4
}
