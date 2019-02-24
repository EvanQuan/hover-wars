
// NEON green color
uniform vec4 vColor;

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main(void)
{
    color = vColor;
}
