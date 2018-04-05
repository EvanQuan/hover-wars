#version 430 core

uniform mat4 modelview;
uniform mat4 projection;

layout (location = 0) in vec3 vertex;

out vec3 vVert;

void main(void)
{
	vVert = vertex;
    gl_Position = projection * modelview * vec4(vertex, 1.0);
}
