#version 430 core

out vec4 color;

uniform vec3 vColor;

void main(void)
{    
    color = vec4( 0.85, 0.85, 0.85, 1.0 );	// Light Grey
	color = vec4( vColor, 1.0 );
}
