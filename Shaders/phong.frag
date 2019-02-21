
out vec4 color;

in VS_OUT {
	vec3 NormalVector;
	vec3 vFragPosition;
	vec3 ToCamera;
	vec4 FragPosDirLightSpace;
	vec4 FragPosSpotLightSpace[4];
} fs_in;

void main(void)
{
	// For alpha blending, discard fragments that are below a certain transparency threshold
	if( texture(sMaterial.vDiffuse, TexCoords).a < 0.1 )
		discard;
		
	// result
	vec4 vColorLinear = vec4(0.0);
	
	// add directional light's contribution
	if( usingDirectionalLight )
		vColorLinear += CalcDirLight( pDirectionalLight, fs_in.NormalVector, fs_in.ToCamera, fs_in.FragPosDirLightSpace );
		
	// add Point Light contributions
	for( int i = 0; i < numPointLights; i++ )
		vColorLinear += CalcPointLight( pPointLights[i], fs_in.NormalVector, fs_in.vFragPosition, fs_in.ToCamera );
	
	// add Spot Light contributions
	for( int i = 0; i < numSpotLights; i++ )
		vColorLinear += CalcSpotLight( pSpotLights[i], fs_in.NormalVector, fs_in.vFragPosition, fs_in.ToCamera );
	
	// Calculate the final color of the fragment
    color = vColorLinear;
	//color = vec4(vec3(texture(DirectionalLightShadow, TexCoords).r), 1.0);
}
