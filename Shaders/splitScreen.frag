
// Texture to sample from
uniform sampler2D hdrBuffer;

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main()
{
	const float fGamma = 2.2;
	const float fExposure = 5.0;
	vec3 vHDRColor = texture(hdrBuffer, TexCoords).rgb;
	
	//vec3 vResult = pow(vHDRColor, vec3(1.0 / fGamma));
	vec3 vResult = vec3(1.0) - exp(-vHDRColor * fExposure);
	vResult = pow(vResult, vec3(1.0 / fGamma));
    color = vec4(vResult, 1.0);
}
