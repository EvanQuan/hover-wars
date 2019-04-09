
void main(void)
{    
	vFragColor = texture(sMaterial.vDiffuse, TexCoords);
	vBrightColor = vFragColor;
}
