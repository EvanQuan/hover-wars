
uniform bool bUsingLinux = true;
float Rval = 1.0;	// Determines the Magnitude of the xtoon effect

out vec4 color;

in vec3 NormalVector;
in vec3 ToCameraVector;
in vec3 vFragPos;

uniform sampler2D gSampler;

void main(void)
{
	vec2 UV;
	vec3 vLightPos = usingDirectionalLight ? -pDirectionalLight.vDirection : pPointLights[0].vPosition;
	vec4 lightCameraSpace = modelview * vec4(vLightPos, 1.0);
	lightCameraSpace /= lightCameraSpace.w;

	vec3 ToLightVector = normalize(lightCameraSpace.xyz - vFragPos);
	
	// Calculate UVs for Orientation-Based Texture Mapping
	UV.y = clamp(pow(abs(dot(NormalVector, ToCameraVector)), Rval), 0.0, 1.0);
	UV.x = clamp( dot(NormalVector, ToLightVector), 0.0, 1.0 );

	if( bUsingLinux )
		UV = -UV;

	vec4 vObjColor = texture(sMaterial.vDiffuse, UV);

    color = vec4(vObjColor.xyz, 1.0);
}
