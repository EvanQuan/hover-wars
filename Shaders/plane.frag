
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
	vec4 plane_color;
	if( !bTextureLoaded )
		plane_color = vec4( 0.133, 0.545, 0.133, 1.0 );
	//else
		plane_color = texture(sMaterial.vDiffuse,TexCoords);

	vec3 vColorLinear = vec3(0.0);
	
	// add directional light's contribution
	if( usingDirectionalLight )
		vColorLinear += CalcDirLight( pDirectionalLight, NormalVector, ToCamera );
		
	// add Point Light contributions
	for( int i = 0; i < numPointLights; i++ )
		vColorLinear += CalcPointLight( pPointLights[i], NormalVector, vFragPosition, ToCamera );
	
	// Calculate the final color of the fragment
    //color = vec4(vColorLinear, 1.0);
    //float kd = 0.8;
    //vec3 diffuse = kd*plane_color.rgb*vLightColor.rgb*max( 0.0, dot( N, normalize(L - P)));
    FragColor = vec4( vColorLinear, 1.0 );
}
