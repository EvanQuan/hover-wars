
out vec4 color;

void main(void)
{    
	color = texture(sMaterial.vDiffuse, TexCoords);
}
