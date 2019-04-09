
void main(void)
{    
	vFragColor = texture(sMaterial.vDiffuse, TexCoords);
	calcBrightColor( vFragColor );
}
