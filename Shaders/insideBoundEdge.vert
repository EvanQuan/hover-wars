#version 430 core

uniform mat4 modelview;
uniform mat4 projection;

uniform mat4 translate;
uniform float fScale;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

out vec3 V;
out vec3 N;
out vec3 ObjN;

void main(void)
{
	V = vertex;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	N = normalize( normalMatrix * normal );
	ObjN = normalize( normal );
	
    gl_Position = translate * vec4( vertex, 1.0 ) * fScale;
}
