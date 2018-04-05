#version 430 core

uniform mat4 modelview;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform float fScale;
uniform mat4 translate;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

//attributes in camera coordinates
out vec3 N;
out vec3 L;
out vec3 V;

void main(void)
{
	vec4 lightCameraSpace = modelview * vec4(lightPosition, 1.0);
	lightCameraSpace /= lightCameraSpace.w;
	   
    vec3 positionModelSpace = (vec4( vertex, 0.0 )).xyz;
	positionModelSpace = (translate * vec4(positionModelSpace, 1.0) ).xyz;
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);
	vec3 P = positionCameraSpace.xyz/positionCameraSpace.w;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	N = normalize( normalMatrix * normal );
	
	L = normalize(lightCameraSpace.xyz - P);
	
	V = normalize( - P);
	
    gl_Position = projection * positionCameraSpace;    
}
