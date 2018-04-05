#version 430 core

out vec4 color;

in vec3 N;
in vec3 P;
in vec3 L;
in vec2 texturePos;

uniform sampler2D gSampler;
uniform bool bTextureLoaded = true;

void main (void)
{
	vec4 plane_color;
	if( !bTextureLoaded )
		plane_color = vec4( 0.133, 0.545, 0.133, 1.0 );
	else
		plane_color = texture(gSampler,texturePos);

    float kd = 0.8;
    vec3 diffuse = kd*plane_color.rgb*max( 0.0, dot( N, normalize(L - P)));
    color = vec4( diffuse, 1.0 );
}
