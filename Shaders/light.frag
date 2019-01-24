
out vec4 color;

uniform vec3 vLightColor;
uniform sampler2D gSampler;

in vec3 vNormalInCameraSpace;
in vec3 V;

void main(void)
{    
	color = texture(sMaterial.vDiffuse, TexCoords);
}
