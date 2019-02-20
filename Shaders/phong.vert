
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 translate;

//attributes in camera coordinates
out VS_OUT {
	vec3 NormalVector;
	vec3 vFragPosition;
	vec3 ToCamera;
	vec4 FragPosDirLightSpace;
	vec4 FragPosSpotLightSpace[4];
} vs_out;

// Tex Coords to send to Lighting Header
out vec2 TexCoords;

void main(void)
{
	vec3 positionModelSpace = ( translate * vec4(vertex, 1.0) ).xyz;
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);
	vec3 P = positionCameraSpace.xyz/positionCameraSpace.w;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	vs_out.NormalVector = normalize( normalMatrix * normal );
	
	vs_out.ToCamera = normalize( - P);
	
	vs_out.vFragPosition = P;
	
	// Calculate the Directional Light Fragment for Shadows
	if( usingDirectionalLight )
		vs_out.FragPosDirLightSpace = dirLightSpaceMat * vec4(vs_out.vFragPosition, 1.0);
	
	// Calculate the Spot Light Fragment for Shadows
	for( int i = 0; i < numSpotLights; ++i )
		vs_out.FragPosSpotLightSpace[i] = spotLightSpaceMat[i] * vec4(vs_out.vFragPosition, 1.0);
	
	TexCoords = uv;
	
    gl_Position = projection * positionCameraSpace;    
}
