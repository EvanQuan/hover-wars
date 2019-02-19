#include "ShaderManager.h"

/////////////
// Defines //
/////////////
#define DIRECTIONAL_LIGHT_OFFSET        16
#define NUM_DIRECTIONAL_LIGHT_PARAMS    4
#define DIRECTIONAL_LIGHT_SIZE          (sizeof(vec4) * NUM_DIRECTIONAL_LIGHT_PARAMS)
#define POINT_LIGHT_OFFSET              (DIRECTIONAL_LIGHT_OFFSET + DIRECTIONAL_LIGHT_SIZE)
// POINT_LIGHT_SIZE: float bytesize of 4 -> 16 bytes due to spacing requirements of uniform buffers
#define NUM_POINT_LIGHT_PARAMS          3
#define POINT_LIGHT_SIZE                (sizeof(vec4) * NUM_POINT_LIGHT_PARAMS)
#define MAX_NUM_POINT_LIGHTS            4
#define NUM_SPOT_LIGHT_PARAMS           4
#define SPOT_LIGHT_SIZE                 (sizeof(vec4) * NUM_SPOT_LIGHT_PARAMS)
#define SPOT_LIGHT_OFFSET               (POINT_LIGHT_OFFSET + (POINT_LIGHT_SIZE * MAX_NUM_POINT_LIGHTS))
#define LIGHT_BUFFER_SIZE               (DIRECTIONAL_LIGHT_OFFSET + DIRECTIONAL_LIGHT_SIZE + (POINT_LIGHT_SIZE << 2) + (SPOT_LIGHT_SIZE << 2))
#define MAX_NUM_SPOT_LIGHTS             4

///////////////
// Constants //
///////////////
const GLsizei INSTANCE_STRIDE = sizeof(mat4);

// Singleton Variable initialization
ShaderManager* ShaderManager::m_pInstance = nullptr;

typedef ShaderManager::eShaderType shader_Type;

// Initialize the Shader Type Hash Map with whatever potential shaders are desired to be accessed.
const unordered_map<string, ShaderManager::eShaderType> ShaderManager::pShaderTypeMap =
{
    make_pair<string, eShaderType>("light_shdr", shader_Type::LIGHT_SHDR),
    make_pair<string, eShaderType>("toon_shdr", shader_Type::TOON_SHDR),
    make_pair<string, eShaderType>("blinn_phong_shdr", shader_Type::BLINN_PHONG_SHDR),
    make_pair<string, eShaderType>("plane_shdr", shader_Type::PLANE_SHDR),
    make_pair<string, eShaderType>("world_shdr", shader_Type::WORLD_SHDR),
    make_pair<string, eShaderType>("boid_shdr", shader_Type::BOID_SHDR),
    make_pair<string, eShaderType>("billboard_shdr", shader_Type::BILLBOARD_SHDR)
};

// Public - Not a singleton
// Designed mainly to manage different shaders between assignments.
ShaderManager::ShaderManager()
{
    m_bInitialized = false;

    // Set up Uniform Buffer Object
    glGenBuffers(1, &m_iMatricesBuffer);
    glGenBuffers(1, &m_iLightsBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_iMatricesBuffer);
    glBufferData(GL_UNIFORM_BUFFER, (sizeof(mat4) * 3), NULL, GL_STATIC_DRAW);    // Allocate 150 bytes of memory
    glBindBuffer(GL_UNIFORM_BUFFER, m_iLightsBuffer);
    glBufferData(GL_UNIFORM_BUFFER, LIGHT_BUFFER_SIZE, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Bind the Uniform Buffer to a base of size 2 * sizeof(mat4) => (Projection and Model View Matrix)
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_iMatricesBuffer, 0, (sizeof(mat4) * 3));    // Bind this buffer base to 0; this is for general Matrices.
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, m_iLightsBuffer, 0, LIGHT_BUFFER_SIZE);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set Edge Shader Locations
    m_pShader[eShaderType::LIGHT_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/light.vert");
    m_pShader[eShaderType::LIGHT_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/light.frag");

    m_pShader[eShaderType::PARTICLE_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/particle.vert");
    m_pShader[eShaderType::PARTICLE_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/particle.frag");

    m_pShader[eShaderType::TOON_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/toon.vert");
    m_pShader[eShaderType::TOON_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/toon.frag");

    m_pShader[eShaderType::BLINN_PHONG_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/phong.vert");
    m_pShader[eShaderType::BLINN_PHONG_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/phong.frag");

    m_pShader[eShaderType::PLANE_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/plane.vert");
    m_pShader[eShaderType::PLANE_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/plane.frag");

    m_pShader[eShaderType::WORLD_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/world.vert");
    m_pShader[eShaderType::WORLD_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/world.frag");

    // RC_Track Shaders
    m_pShader[eShaderType::BILLBOARD_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/billboard.vert");
    m_pShader[eShaderType::BILLBOARD_SHDR].storeShadrLoc(Shader::eShader::GEOMETRY, "Shaders/billboard.geo");
    m_pShader[eShaderType::BILLBOARD_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/billboard.frag");

    m_pShader[eShaderType::BOID_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/boid.vert");
    m_pShader[eShaderType::BOID_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/boid.frag");

    // Bounding Box Shader
    m_pShader[eShaderType::DEBUG_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/debug.vert");
    m_pShader[eShaderType::DEBUG_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/debug.frag");

    // Shadow Shader
    m_pShader[eShaderType::SHADOW_SHDR].storeShadrLoc(Shader::eShader::VERTEX, "Shaders/shadow.vert");
    m_pShader[eShaderType::SHADOW_SHDR].storeShadrLoc(Shader::eShader::FRAGMENT, "Shaders/shadow.frag");
}

// Get the Singleton ShaderManager Object.  Initialize it if nullptr.
ShaderManager* ShaderManager::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new ShaderManager();
    }

    return m_pInstance;
}

// Destructor - Kill any shaders that we've been using.
ShaderManager::~ShaderManager()
{
    // unbind any shader programs
    glUseProgram(0);
    glDisable(GL_BLEND);

    glDeleteBuffers(1, &m_iMatricesBuffer);
    glDeleteBuffers(1, &m_iLightsBuffer);
}

// Given a potential string as a hashmap key, return the corresponding ShaderType
//    Returns: A found ShaderType or Default is Plane Shader.
shader_Type ShaderManager::getShaderType(const string& sKey)
{
    shader_Type eReturnType = PLANE_SHDR;

    if (pShaderTypeMap.end() != pShaderTypeMap.find(sKey))
    {
        eReturnType = pShaderTypeMap.at(sKey);
    }

    return eReturnType;
}

/*******************************************************************\
 * Set up Shaders                                                  *
\*******************************************************************/

// Inializes shaders.
bool ShaderManager::initializeShaders()
{
    // Initialize Shaders
    m_bInitialized = true;
    for (int eIndex = LIGHT_SHDR; eIndex < MAX_SHDRS; eIndex++)
    {
        m_bInitialized &= m_pShader[eIndex].initializeShader();
    }

    // return False if not all Shaders Initialized Properly
    return m_bInitialized;
}


/*******************************************************************************\
* Shader manipulation                                                          *
\*******************************************************************************/

// Set Projection Matrix for all Shaders
void ShaderManager::setProjectionModelViewMatrix(const mat4* pProjMat, const mat4* pModelViewMat)
{
    mat4 pInvModelViewMat = inverse(*pModelViewMat);
    glBindBuffer(GL_UNIFORM_BUFFER, m_iMatricesBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), pProjMat);                                  // Set the Projection Matrix Data to the uniform Buffer.
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), pModelViewMat);                  // Set the Model View Matrix Data to the uniform Buffer.
    glBufferSubData(GL_UNIFORM_BUFFER, (sizeof(mat4) * 2), sizeof(mat4), &pInvModelViewMat);        // Set the Inverse Model View Matrix Data for getting Camera Position.
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::setLightsInUniformBuffer(const LightingComponent* pDirectionalLight, const vector< LightingComponent* >* pPointLights)
{
    // Get initial values for Light Block
    int bUsingDirectionalLight = nullptr != pDirectionalLight;
    unsigned int iNumPointLights = 0, iNumSpotLights = 0;
    vector< vec4 > vPointLightData, vSpotLightData;
    const vector< vec4 > *pDirectionalLightData = nullptr, *pPointLightData = nullptr, *pSpotLightData = nullptr;

    if (bUsingDirectionalLight)
    {
        pDirectionalLightData = pDirectionalLight->getLightInformation();
        assert(pDirectionalLightData->size() == NUM_DIRECTIONAL_LIGHT_PARAMS);
    }

    // Pull the data from the Lighting Components
    for (vector< LightingComponent* >::const_iterator iter = pPointLights->begin();
        iter != pPointLights->end();
        ++iter)
    {
        // POINT_LIGHT - Limited by a maximum number of Point Lights to store in the buffer.
        if (MAX_NUM_POINT_LIGHTS > iNumPointLights &&
            LightingComponent::eLightType::POINT_LIGHT == (*iter)->getType())
        {
            // Get Information
            pPointLightData = (*iter)->getLightInformation();

            // Returned amount of information should be expected.
            assert(pPointLightData->size() == NUM_POINT_LIGHT_PARAMS);
            vPointLightData.insert(vPointLightData.end(), pPointLightData->begin(), pPointLightData->end());
            ++iNumPointLights;        // count this as an added Point Light
        }
        else if (MAX_NUM_SPOT_LIGHTS > iNumSpotLights &&
            LightingComponent::eLightType::SPOTLIGHT == (*iter)->getType())
        {
            // Get Information
            pSpotLightData = (*iter)->getLightInformation();

            // Assert Expectations
            assert(pSpotLightData->size() == NUM_SPOT_LIGHT_PARAMS);
            vSpotLightData.insert(vSpotLightData.end(), pSpotLightData->begin(), pSpotLightData->end());
            ++iNumSpotLights;

        }
    }

    //  GLSL Uniform Buffer Block:
    //      Base Alignment = the space a variable takes (including padding) within a uniform block. Per std140 layout rules.
    //      Aligned Offset = the byte offset of a variable from the start of the block. Must be equal to a multiple of its base alignment.
    //      For Light Buffer:
    //          int iNumLights                  Base: 4     Aligned: 0
    //          bool bUsingDirectionalLight     Base: 4     Aligned: 4  // Booleans are stored in 4 bytes in GLSL
    //          vec3 pDirectionalLight[4]       Base: 64    Aligned: 16 // vec3 stored with 4 bytes of padding; must be aligned to base alignment
    //          vec3 pPointLight[2]             Base: 32    Aligned: 80
    glBindBuffer(GL_UNIFORM_BUFFER, m_iLightsBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &iNumPointLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &iNumSpotLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &bUsingDirectionalLight);
    if (bUsingDirectionalLight)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, DIRECTIONAL_LIGHT_OFFSET, NUM_DIRECTIONAL_LIGHT_PARAMS * sizeof(vec4), pDirectionalLightData->data());
    }
    glBufferSubData(GL_UNIFORM_BUFFER, POINT_LIGHT_OFFSET, (NUM_POINT_LIGHT_PARAMS * sizeof(vec4))*iNumPointLights, vPointLightData.data());
    glBufferSubData(GL_UNIFORM_BUFFER, SPOT_LIGHT_OFFSET, (NUM_SPOT_LIGHT_PARAMS * sizeof(vec4))*iNumSpotLights, vSpotLightData.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Binds and creates a buffer on the GPU.  Sets the data into the buffer and returns the location of the buffer.
GLuint ShaderManager::genVertexBuffer(GLuint iVertArray, const void* pData, GLsizeiptr pSize, GLenum usage)
{
    GLuint iVertexBufferLoc;
    glBindVertexArray(iVertArray);
    glGenBuffers(1, &iVertexBufferLoc);
    glBindBuffer(GL_ARRAY_BUFFER, iVertexBufferLoc);
    glBufferData(GL_ARRAY_BUFFER, pSize, pData, usage);

    //glBindVertexArray( 0 );
    return iVertexBufferLoc;
}

// Enables an Attribute Pointer for a specified Vertex Array
void ShaderManager::setAttrib(GLuint iVertArray, GLuint iSpecifiedIndex, GLint iChunkSize, GLsizei iStride, const void *pOffset)
{
    glBindVertexArray(iVertArray);
    glVertexAttribPointer(iSpecifiedIndex, iChunkSize, GL_FLOAT, GL_FALSE, iStride, pOffset);
    glEnableVertexAttribArray(iSpecifiedIndex);

    //glBindVertexArray(0);
}

// Binds and creates an Element Array Buffer on the GPU.  Sets the data into the buffer and returns the location.
GLuint ShaderManager::genIndicesBuffer(GLuint iVertArray,
    const void* pData, GLsizeiptr pSize, GLenum usage)
{
    GLuint iIndicesBufferLoc;

    glBindVertexArray(iVertArray);

    glGenBuffers(1, &iIndicesBufferLoc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIndicesBufferLoc);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pSize, pData, usage);

    //glBindVertexArray( 0 );

    return iIndicesBufferLoc;
}

// Function to Generate an Instance Buffer for Instanced Rendering.
GLuint ShaderManager::genInstanceBuffer(GLuint iVertArray, GLuint iStartIndex, const void* pData, GLsizeiptr pSize, GLenum usage)
{
    // Set up Instanced Buffer for Instance Rendering
    GLuint iReturnIBO = genVertexBuffer(iVertArray, pData, pSize, usage);

    // Instance Rendering Attributes
    //    Set up openGL for referencing the InstancedBuffer as a Mat4
    // column 0
    glBindBuffer(GL_ARRAY_BUFFER, iReturnIBO);
    setAttrib(
        iVertArray, iStartIndex, 4, INSTANCE_STRIDE, (void*)0);
    // column 1
    setAttrib(
        iVertArray, iStartIndex + 1, 4, INSTANCE_STRIDE, (void*)sizeof(vec4));
    // column 2
    SHADER_MANAGER->setAttrib(
        iVertArray, iStartIndex + 2, 4, INSTANCE_STRIDE, (void*)(sizeof(vec4) << 1));
    // column 3
    SHADER_MANAGER->setAttrib(
        iVertArray, iStartIndex + 3, 4, INSTANCE_STRIDE, (void*)(3 * sizeof(vec4)));

    glBindVertexArray(iVertArray);
    glVertexAttribDivisor(iStartIndex, 1);
    glVertexAttribDivisor(iStartIndex + 1, 1);
    glVertexAttribDivisor(iStartIndex + 2, 1);
    glVertexAttribDivisor(iStartIndex + 3, 1);

    return iReturnIBO;
}

// given a glm 4x4 Matrix, a specifed shader and a variablename, attempt to set the given matrix into that uniform variable.
void ShaderManager::setUnifromMatrix4x4(eShaderType eType, string sVarName, const mat4* pResultingMatrix)
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);

        glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
        glUseProgram(iProgram);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());
        if (ERR_CODE != iVariableLocation)
        {
            glUniformMatrix4fv(iVariableLocation, 1, GL_FALSE, value_ptr(*pResultingMatrix));
        }
        glUseProgram(iCurrProgram);

#ifdef DEBUG
        CheckGLErrors();
#endif // DEBUG
    }
}

// given a glm vec3 set it as the unifrom light position in the mesh shader
void ShaderManager::setUniformVec3(eShaderType eType, string sVarName, const glm::vec3* pValue )
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);

        glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
        glUseProgram(iProgram);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());
        if (ERR_CODE != iVariableLocation)
        {
            glUniform3fv(iVariableLocation, 1, glm::value_ptr(*pValue));
        }
        glUseProgram(iCurrProgram);

#ifdef DEBUG
        CheckGLErrors();
#endif // DEBUG
    }
}

// sets a uniform vec4 value in the specified shader to the given value.
void ShaderManager::setUniformVec4(eShaderType eType, string sVarName, const vec4* pValue)
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);

        glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
        glUseProgram(iProgram);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());
        if (ERR_CODE != iVariableLocation)
        {
            glUniform4fv(iVariableLocation, 1, glm::value_ptr(*pValue));
        }
        glUseProgram(iCurrProgram);

#ifdef DEBUG
        CheckGLErrors();
#endif // DEBUG
    }
}

// Sets a single uniform floating value in the specified shader to the given value.
void ShaderManager::setUniformFloat(eShaderType eType, string sVarName, float fVal)
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);

        glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
        glUseProgram(iProgram);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());
        if (ERR_CODE != iVariableLocation)
        {
            glUniform1f(iVariableLocation, fVal);
        }
        glUseProgram(iCurrProgram);

#ifdef DEBUG
        CheckGLErrors();
#endif // DEBUG
    }
}

// Sets a uniform integer value in the specified shader program to the given value.
void ShaderManager::setUniformInt(eShaderType eType, string sVarName, int iVal)
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);

        glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
        glUseProgram(iProgram);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());
        if (ERR_CODE != iVariableLocation)
        {
            glUniform1i(iVariableLocation, iVal);
        }
        glUseProgram(iCurrProgram);

#ifdef DEBUG
        CheckGLErrors();
#endif // DEBUG
    }
}

// Set a Uniform Boolean Value within a given Shader.
void ShaderManager::setUniformBool(eShaderType eType, string sVarName, bool bVal)
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());

        if (ERR_CODE != iVariableLocation)
        {
            glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
            glUseProgram(iProgram);
            glUniform1i(iVariableLocation, bVal);
            glUseProgram(iCurrProgram);
        }
    }
}

// Toggles a Boolean Value in the given shader.
void ShaderManager::toggleUniformBool(eShaderType eType, string sVarName)
{
    GLint iVariableLocation;
    GLint iProgram, iCurrProgram;
    GLint bCurrSetting;

    if (eType < eShaderType::MAX_SHDRS && eType >= 0)
    {
        iProgram = getProgram(eType);
        iVariableLocation = glGetUniformLocation(iProgram, sVarName.c_str());

        if (ERR_CODE != iVariableLocation)
        {
            glGetUniformiv(iProgram, iVariableLocation, &bCurrSetting);

            bCurrSetting = !bCurrSetting;

            glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrProgram);
            glUseProgram(iProgram);
            glUniform1i(iVariableLocation, bCurrSetting);
            glUseProgram(iCurrProgram);
        }
    }
}
