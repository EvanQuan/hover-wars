
// Texture to sample from
uniform sampler2D text;

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main()
{
    color = texture(text, TexCoords);
}
