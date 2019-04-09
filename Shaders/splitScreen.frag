
// Texture to sample from
uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;

// Main: just output the color of the fragment
void main()
{
	const float fGamma = 2.2;
	const float fExposure = 1.0;
	vec3 vHDRColor = texture(hdrBuffer, TexCoords).rgb;
	vec3 vBloomColor = texture(bloomBuffer, TexCoords).rgb;
	
	vec3 vResult = vec3(1.0) - exp(-vBloomColor * fExposure);
	vResult = pow(vResult, vec3(1.0 / fGamma));
    vFragColor = vec4(vResult + ((1 - vResult) * vHDRColor), 1.0);
}
