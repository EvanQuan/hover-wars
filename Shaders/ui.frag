// Input color
uniform vec3 textColor;
// Texture to sample from
uniform sampler2D text;	

// 

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main()
{
    color = vec4(textColor, texture(text, TexCoords).r);
	// use uniform boolean to determine whether an image or text is being rendered
	// ShaderManager will set this value
	//color = texture(text, TexCoords);
}
