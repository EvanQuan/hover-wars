
out vec4 color;

in vec3 NormalVector;
in vec3 vFragPosition;
in vec3 ToCamera;
in float distanceToLight;

void main(void)
{
	// For alpha blending, discard fragments that are below a certain transparency threshold
	if( texture(sMaterial.vDiffuse, TexCoords).a < 0.1 )
		discard;
		
	// result
	vec4 vColorLinear = vec4(0.0);
	
	// add directional light's contribution
	if( usingDirectionalLight )
		vColorLinear += CalcDirLight( pDirectionalLight, NormalVector, ToCamera );
		
	// add Point Light contributions
	for( int i = 0; i < numPointLights; i++ )
		vColorLinear += CalcPointLight( pPointLights[i], NormalVector, vFragPosition, ToCamera );
	
	// add Spot Light contributions
	for( int i = 0; i < numSpotLights; i++ )
		vColorLinear += CalcSpotLight( pSpotLights[i], NormalVector, vFragPosition, ToCamera );
	
	// Calculate the final color of the fragment
    color = vColorLinear;
}
