
out vec4 color;

void main(void)
{    
	vec4 vFragColor = texture(sMaterial.vDiffuse, TexCoords );
	
	if( vFragColor.a < 0.1 )
		discard;
		
	float fK = (vFragColor.r * 0.3) + (0.59 * vFragColor.g) + (0.11 * vFragColor.b);
	if( fK < 0.1 )
		discard;
		
	color = vFragColor;
}
