
// NEON green color
uniform vec3 vColor = vec3(0.2235294117647059, 1.0, 0.0784313725490196);

// Output Fragment color
out vec4 color;

// Main: just output the color of the fragment
void main(void)
{    
    color = vec4( vColor, 1.0 );
}
