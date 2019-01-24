#version 430 core

// Structures for Different Light Sources
struct DirLight {
	vec3 vDirection;
	vec3 vAmbient;
	vec3 vDiffuse;
	vec3 vSpecular;
};

struct PointLight {
	vec3 vPosition;
	vec3 vColor;
};

#define MAX_POINT_LIGHTS 4

layout (std140, binding = 2) uniform Lights
{
	int numPointLights;
	bool usingDirectionalLight;
	DirLight pDirectionalLight;
	PointLight pPointLights[4];
};

// Gooch Variables
float fShine = 5.0;	// Specular "Shininess" value

const vec3 MIDNIGHT_BLUE = vec3(0.09803921568627450980392156862745, 0.09803921568627450980392156862745, 0.43921568627450980392156862745098); 
out vec4 color;

in vec3 NormalVector;
in vec3 LightVector;
in vec3 ToCamera;
in float distanceToLight;

uniform sampler2D gSampler;
uniform vec3 lightPosition;
uniform vec3 ambientColor = vec3( 0.0, 0.0, 0.2 );
uniform vec3 diffuseColor = MIDNIGHT_BLUE;
uniform vec3 specColor = vec3( 1.0 );
vec3 lightColor = vec3( 1.0 );
uniform float lightPower = 4.0;

//Blinn-Phong calculations adapted from: 
//	goo.gl/vzRHX2 (Wikipedia for Blinn-Phong)
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
	lightColor = Lights.pPointLights[0].vColor;
	
	// Calculate Blinn-Phong Lambertian value.
	float lambertian = max(dot(LightVector, NormalVector), 0.0);
	float specular = 0.0;

	if( lambertian > 0.0) 
	{
		// Blinn-Phong Calculation
		vec3 halfDir = normalize( LightVector + ToCamera );
		float specAngle = max( dot(halfDir, NormalVector), 0.0);
		specular = pow( specAngle, fShine);
	}

	vec3 colorLinear = ambientColor + 
						diffuseColor * lambertian * lightColor * lightPower / distance +
						specColor * specular * lightColor * lightPower / distance;
	// Calculate the final color of the fragment
    color = vec4(colorLinear, 1.0);
}
