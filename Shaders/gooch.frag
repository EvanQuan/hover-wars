#version 430 core

// Gooch Variables
float b = 0.5;		// Determine maxmimum "blueness" in temperature shift
float y = 0.5;		// Determine maximmum "yellowness" in temperature shift
float alpha = 0.5;	// Amount of which the Color is visible in the Cool Temperature
float beta = 0.5;	// Amount of which the Color is visible in the Warm Temperature
float fShine = 15.0;	// Specular "Shininess" value

const vec3 MIDNIGHT_BLUE = vec3(0.09803921568627450980392156862745, 0.09803921568627450980392156862745, 0.43921568627450980392156862745098); 
out vec4 color;

in vec3 NormalVector;
in vec3 LightVector;
in vec3 ToCamera;

uniform sampler2D gSampler;

void main(void)
{
	vec2 UV;
	vec4 vObjColor = vec4( MIDNIGHT_BLUE, 1.0 );
	vec3 kCool;
	vec3 kWarm;
	vec4 textureColor;
	vec3 diffuse = vec3(0.0);
	float specular = 1.0;
	float e = fShine;

	vec3 ReflectionVector = reflect(-LightVector, NormalVector);

	kCool = vec3( 0.0, 0.0, b) + (alpha * vObjColor.rgb);
	kWarm = vec3( y, y, 0.0 ) + (beta * vObjColor.rgb);

	// Implementing Gooch Shading:
	//		Formula: I = (( 1 - (LightVector.NormalVector))/2) * kCool +
	//					  (1 - (1 - (LightVector.NormalVector))/2) * kWarm
	float fDotCalc = clamp((1.0 - dot(NormalVector,LightVector)) / 2.0, 0.0, 1.0);
	diffuse = (fDotCalc * kCool) + ((1 - fDotCalc) * kWarm);
	specular = pow(max( dot(ReflectionVector,ToCamera), 0.0 ), e);

	// Calculate the final color of the fragment
    color = vec4(clamp(diffuse + vObjColor.xyz*specular, 0.0, 1.0), 1.0);
}
