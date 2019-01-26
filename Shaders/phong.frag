
// Gooch Variables
float fShine = 0.5;	// Specular "Shininess" value

const vec3 MIDNIGHT_BLUE = vec3(0.09803921568627450980392156862745, 0.09803921568627450980392156862745, 0.43921568627450980392156862745098); 
out vec4 color;

in vec3 NormalVector;
in vec3 vFragPosition;
in vec3 ToCamera;
in float distanceToLight;

uniform sampler2D gSampler;
uniform vec3 lightPosition;
uniform vec3 ambientColor = vec3( 0.0, 0.0, 0.2 );
uniform vec3 diffuseColor = MIDNIGHT_BLUE;
uniform vec3 specColor = vec3( 1.0 );
vec3 lightColor = vec3( 1.0 );
uniform float lightPower = 4.0;

void main(void)
{
	// Local Variables
	vec2 UV;
	vec4 vObjColor = vec4( MIDNIGHT_BLUE, 1.0 );
	vec3 kCool;
	vec3 kWarm;
	vec4 textureColor;
	vec3 diffuse = vec3(0.0);
	float e = fShine;
	float distance = distanceToLight * distanceToLight;
	vec3 vColorLinear = vec3(0.0);
	
	// add directional light's contribution
	if( usingDirectionalLight )
		vColorLinear += CalcDirLight( pDirectionalLight, NormalVector, ToCamera );
		
	// add Point Light contributions
	for( int i = 0; i < numPointLights; i++ )
		vColorLinear += CalcPointLight( pPointLights[i], NormalVector, vFragPosition, ToCamera );
	
	// Calculate the final color of the fragment
    color = vec4(vColorLinear, 1.0);
}
