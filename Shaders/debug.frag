
// NEON green color
uniform vec4 vColor;

// Main: just output the color of the fragment
void main(void)
{
    vFragColor = vColor;
	vBrightColor = vec4(0.0, 0.0, 0.0,0.0);
}
