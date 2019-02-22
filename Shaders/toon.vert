
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
out vec3 NormalVector;
out vec3 ToCameraVector;
out vec3 vFragPos;

void main(void)
{
	vec3 positionModelSpace = ( translate * vec4(vertex, 1.0) ).xyz;
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);
	vFragPos = positionCameraSpace.xyz/positionCameraSpace.w;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	NormalVector = normalize( normalMatrix * normal );
	
	ToCameraVector = normalize( - vFragPos);
	
    gl_Position = projection * positionCameraSpace;    
}
