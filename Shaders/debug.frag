
// NEON green color
uniform vec3 vColor;

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main(void)
{    
    color = vec4( vColor, 1.0 );
}
