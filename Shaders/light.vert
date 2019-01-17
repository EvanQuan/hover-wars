#version 430 core

uniform mat4 modelview;
uniform mat4 projection;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 V;
out vec3 vNormalInCameraSpace;

void main(void)
{
	vec3 vPositionModelSpace = (vec4( vertex, 0.0 )).xyz;
	vec4 vPositionCameraSpace = modelview * vec4(vPositionModelSpace, 1.0);
	vec3 P = vPositionCameraSpace.xyz/vPositionCameraSpace.w;
	
	// Normal Matrix to correct Normal in camera space
	mat3 m3NormalMatrix = transpose( inverse( mat3( modelview ) ) );
	vNormalInCameraSpace = normalize( m3NormalMatrix * normal );
	V = normalize( -P );
    gl_Position = projection * vPositionCameraSpace;
}
