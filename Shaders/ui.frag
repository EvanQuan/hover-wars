// Input color
uniform vec3 textColor;
// Texture to sample from
uniform sampler2D text;	

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main()
{
    color = vec4(textColor, texture(text, TexCoords).r);
	//color = vec4(1.0);
}
