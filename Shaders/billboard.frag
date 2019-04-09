
in vec3 vFragTinge;

void main(void)
{    
	vec4 vColor = texture(sMaterial.vDiffuse, TexCoords );
	
	if( vColor.a < 0.1 )
		discard;
		
	float fK = (vColor.r * 0.3) + (0.59 * vColor.g) + (0.11 * vColor.b);
	if( fK < 0.1 )
		discard;
	
	float fBrightness = dot(THRESHOLD, vColor.rgb);
	vColor.rgb = vec3(fBrightness) * vFragTinge; // Tinge the Billboard with a color change.
	vFragColor = vColor;
	
	if( fBrightness > 0.3 )
		vBrightColor = vFragColor;
	else
		vBrightColor = vec4( vec3(0.0), 1.0);
}
