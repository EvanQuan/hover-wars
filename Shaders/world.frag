#version 430 core

uniform vec3 vColor;

out vec4 color;

in vec3 vVert;

void main(void)
{    
    color = vec4( vColor, 1.0 );
}
