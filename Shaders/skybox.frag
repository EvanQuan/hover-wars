
in vec3 vTextureDir;			// 3D texture coordinates
uniform samplerCube cubemap; 	// cubemap texture sampler

void main(void)
{    
    vFragColor = texture( cubemap, vTextureDir );
	calcBrightColor( vFragColor );
}
