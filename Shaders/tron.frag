
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
	vec3 vColorLinear = vec3(0.0);
	
	// add directional light's contribution
	if( usingDirectionalLight )
		vColorLinear += CalcDirLight( pDirectionalLight, fs_in.NormalVector, fs_in.ToCamera, fs_in.FragPosDirLightSpace );
		
	// add Point Light contributions
	for( int i = 0; i < numPointLights; i++ )
		vColorLinear += CalcPointLight( pPointLights[i], fs_in.NormalVector, fs_in.vFragPosition, fs_in.ToCamera );
	
	// add Spot Light contributions
	for( int i = 0; i < numSpotLights; i++ )
		vColorLinear += CalcSpotLight( pSpotLights[i], fs_in.NormalVector, fs_in.vFragPosition, fs_in.ToCamera );
	
	// Calculate the final vFragColor of the fragment
    vFragColor = vec4(vColorLinear, 1.0);
	
	vec3 vTextureColor = texture(sMaterial.vDiffuse, TexCoords).rgb;
	
	if( vec3(0.0) != vTextureColor )
	//if (vTextureColor.r >= 1.0f && vTextureColor.g >= 1.0f && vTextureColor.b >= 1.0f)
		vBrightColor = vec4(vTextureColor, 1.0);
	//else if ( vFragColor.r + vFragColor.g + vFragColor.b > 1.0 )
	//	vBrightColor = vFragColor;
	else
		vBrightColor = vec4(vec3(0.0), 1.0);
}
