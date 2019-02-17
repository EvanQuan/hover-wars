#include "SpatialDataMap.h"
#include "ShaderManager.h"


// Default Constructor for Data Map
SpatialDataMap::SpatialDataMap()
{

}

// Destructor for Data Map
SpatialDataMap::~SpatialDataMap()
{
    // Reset Map
    clearMap();
}

void SpatialDataMap::initializeMap(float fLength, float fWidth, float fTileSize)
{
    // Local Variables
    float fHalfLength = fLength * 0.5f;
    float fHalfWidth = fWidth * 0.5f;
    m_fTileSize = fTileSize;
    m_iMaxX = floor(fWidth / fTileSize);
    m_iMaxY = floor(fLength / fTileSize);

    // Set the Origin position for reference wrt world coordinates as well as the maximum limit of the map.
    m_vOriginPos = vec2(-fHalfWidth, -fHalfLength);
    m_vMaxDimensions = vec2(fWidth, fLength) + m_vOriginPos;
}

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

    // Delete VBOs and VAOs
    glDeleteBuffers(1, &m_iMapIndicesBuffer);
    glDeleteBuffers(1, &m_iMapVertexBuffer);
    glDeleteVertexArrays(1, &m_iMapVertexArray);
}

/*********************************************************************************\
* Private Functions                                                              *
\*********************************************************************************/

void SpatialDataMap::generateVBOs()
{
    // Iterate through all possible vertices and populate the vertices and indices for rendering.
    for( unsigned int x = 0; x <= m_iMaxX; ++x )
        for (unsigned int y = 0; y <= m_iMaxY; ++y)
        {
            // Push back Vertex for this point.
            m_pVertices.push_back(vec3(m_vOriginPos.x + (x * m_fTileSize),
                                       0.0f,
                                       m_vOriginPos.y + (y * m_fTileSize)));

            // Line from current row to next row.
            if (m_iMaxX != x)
            {
                m_pIndices.push_back((x * m_iMaxX) + y);
                m_pIndices.push_back(m_pIndices.back() + m_iMaxX);
            }

            // Line along this row.
            if (m_iMaxY != y)
            {
                m_pIndices.push_back((x * m_iMaxX) + y);
                m_pIndices.push_back(m_pIndices.back() + 1);
            }
        }

    // Generate Vertex Array
    glGenVertexArrays(1, &m_iMapVertexArray);

    // Generate Vertex Buffer
    m_iMapVertexBuffer = SHADER_MANAGER->genVertexBuffer(m_iMapVertexArray, m_pVertices.data(), m_pVertices.size() * sizeof(vec3), GL_STATIC_DRAW);
    SHADER_MANAGER->setAttrib(m_iMapVertexArray, 0, 3, sizeof(vec3), (void*)0);

    // Generate Indices Buffer
    m_iMapIndicesBuffer = SHADER_MANAGER->genIndicesBuffer(m_iMapVertexArray, m_pIndices.data(), m_pIndices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
}
