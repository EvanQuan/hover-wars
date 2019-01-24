#version 430 core

uniform bool bUsingLinux = true;
float Rval = 1.0;	// Determines the Magnitude of the xtoon effect

out vec4 color;

in vec3 NormalVector;
in vec3 ToLightVector;
in vec3 ToCameraVector;

uniform sampler2D gSampler;

void main(void)
{
	vec2 UV;

	// Calculate UVs for Orientation-Based Texture Mapping
	UV.y = clamp(pow(abs(dot(NormalVector, ToCameraVector)), Rval), 0.0, 1.0);
	UV.x = clamp( dot(NormalVector, ToLightVector), 0.0, 1.0 );

	if( bUsingLinux )
		UV = -UV;

	vec4 vObjColor = texture(gSampler, UV);

    color = vec4(vObjColor.xyz, 1.0);
}
