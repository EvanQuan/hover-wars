#version 430 core

out vec4 color;

uniform sampler2D gSampler;

void main(void)
{    
    color = texture( gSampler, gl_PointCoord );
}
