
// Resulting Fragment Color
out vec4 color;

in vec3 vTextureDir;			// 3D texture coordinates
uniform samplerCube cubemap; 	// cubemap texture sampler

void main(void)
{    
    color = texture( cubemap, vTextureDir );
}
