#version 430 core

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
};

uniform vec3 lightPosition;
uniform float fScale;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 translate;

//attributes in camera coordinates
out vec3 NormalVector;
out vec3 LightVector;
out vec3 ToCamera;

void main(void)
{
	vec4 lightCameraSpace = modelview * vec4(lightPosition, 1.0);
	lightCameraSpace /= lightCameraSpace.w;
	vec3 vScaledVertex = vertex * fScale;
    vec3 positionModelSpace = ( vec4( vScaledVertex, 0.0 )).xyz;
	positionModelSpace = ( translate * vec4(positionModelSpace, 1.0) ).xyz;
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);
	vec3 P = positionCameraSpace.xyz/positionCameraSpace.w;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	NormalVector = normalize( normalMatrix * normal );
	
	LightVector = normalize(lightCameraSpace.xyz - P);
	
	ToCamera = normalize( - P);
	
    gl_Position = projection * positionCameraSpace;    
}
