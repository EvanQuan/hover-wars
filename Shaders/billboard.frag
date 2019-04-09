
in vec3 vFragTinge;

vec3 getBWFragment( vec3 vColor )
{
	return vec3(dot(THRESHOLD, vColor));
}

void main(void)
{    
	vec4 vColor = texture(sMaterial.vDiffuse, TexCoords );
	
	if( vColor.a < 0.1 )
		discard;
		
	float fK = (vColor.r * 0.3) + (0.59 * vColor.g) + (0.11 * vColor.b);
	if( fK < 0.1 )
		discard;
		
	vColor.rgb = getBWFragment(vColor.rgb) * vFragTinge; // Tinge the Billboard with a color change.
	vFragColor = vColor;
	
	if( vFragColor.r > 0.3 )
		vBrightColor = vFragColor;
	else
		vBrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
