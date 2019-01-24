#version 430 core

out vec4 FragColor;

in vec3 N;
in vec3 P;
in vec3 L;
in vec2 texturePos;

uniform sampler2D gSampler;
uniform bool bTextureLoaded = true;
uniform vec3 vLightColor = vec3( 1.0 );

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
};

void main (void)
{
	vec4 plane_color;
	if( !bTextureLoaded )
		plane_color = vec4( 0.133, 0.545, 0.133, 1.0 );
	else
		plane_color = texture(gSampler,texturePos);

    float kd = 0.8;
    vec3 diffuse = kd*plane_color.rgb*vLightColor.rgb*max( 0.0, dot( N, normalize(L - P)));
    FragColor = vec4( diffuse, 1.0 );
}
