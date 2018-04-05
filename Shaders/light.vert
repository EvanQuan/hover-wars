#version 430 core

uniform mat4 modelview;
uniform mat4 projection;

uniform vec3 lightPosition;

void main(void)
{
    gl_Position = projection * modelview * vec4(lightPosition, 1.0);
}
