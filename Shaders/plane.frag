
//in vec2 TexCoords;

out vec4 FragColor;

in vec3 NormalVector;
in vec3 vFragPosition;
in vec3 ToCamera;
//in vec2 TexCoords;

uniform bool bTextureLoaded = true;
uniform vec3 vLightColor = vec3( 1.0 );

void main (void)
{
	if( texture(sMaterial.vDiffuse, TexCoords).a < 0.1 )
		discard;

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
		
    FragColor = vColorLinear;
}
