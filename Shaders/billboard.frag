

void main(void)
{    
	vec4 vColor = texture(sMaterial.vDiffuse, TexCoords );
	
	if( vColor.a < 0.1 )
		discard;
		
	float fK = (vColor.r * 0.3) + (0.59 * vColor.g) + (0.11 * vColor.b);
	if( fK < 0.1 )
		discard;
		
	vFragColor = vColor;
	
	if( vFragColor.r > 0.3 )
		vBrightColor = vFragColor;
	else
		vBrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
