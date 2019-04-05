
uniform vec3 vColor;
in vec3 vVert;

void main(void)
{    
    vFragColor = vec4( vVert, 1.0 );
	vBrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}
