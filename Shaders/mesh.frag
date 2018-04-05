#version 430 core

// Gooch Variables
uniform float b = 0.0;		// Determine maxmimum "blueness" in temperature shift
uniform float y = 0.0;		// Determine maximmum "yellowness" in temperature shift
uniform float alpha = 0.5;	// Amount of which the Color is visible in the Cool Temperature
uniform float beta = 0.5;	// Amount of which the Color is visible in the Warm Temperature
uniform float rc = 0.0;		// Red Value of the Object's Color
uniform float gc = 1.0;		// Green Value of the Object's Color
uniform float bc = 0.0;		// Blue Value of the Object's Color

// X-Toon Variables
uniform float Rval = 1.0;	// Determines magnitude of x-toon effect
uniform float fShine = 5.0;	// Specular "Shininess" value

// Booleans
uniform bool useToon = true;	// Enables X-Toon Shading
uniform bool useGooch = false;	// Enables Gooch Shading
uniform bool useSpec = false;	// Enables Specular Shading

uniform bool bUsingLinux = true;

out vec4 color;

in vec3 N;
in vec3 L;
in vec3 V;

uniform sampler2D mySampler;

void main(void)
{
	vec2 UV;
	vec4 vObjColor = vec4( rc, gc, bc, 1.0 );
	vec3 kCool;
	vec3 kWarm;
	vec4 textureColor;
	vec3 diffuse = vec3(0.0);
	float specular = 1.0;
	float e = fShine;

	vec3 R = reflect(-L, N);

	if( !useSpec )
		e = 1.0;

	// Calculate UVs for Orientation-Based Texture Mapping
	if( useToon )	// Toon Shading Enabled? calculate UVs
	{
		if( useSpec )
			UV.y = clamp(pow(abs(dot(R,V)), e), 0.0, 1.0);
		else
			UV.y = clamp(pow(abs(dot(N, V)), Rval), 0.0, 1.0);

		UV.x = clamp( dot(N, L), 0.0, 1.0 );

		if( bUsingLinux )
			UV = -UV;

		vObjColor = texture(mySampler, UV);
	}

	kCool = vec3( 0.0, 0.0, b) + (alpha * vObjColor.rgb);
	kWarm = vec3( y, y, 0.0 ) + (beta * vObjColor.rgb);

	// Implementing Gooch Shading:
	//		Formula: I = (( 1 - (L.N))/2) * kCool +
	//					  (1 - (1 - (L.N))/2) * kWarm
	if( useGooch )
	{
		float fDotCalc = clamp((1.0 - dot(N,L)) / 2.0, 0.0, 1.0);
		diffuse = (fDotCalc * kCool) + ((1 - fDotCalc) * kWarm);
		specular = pow(max( dot(R,V), 0.0 ), e);
	}

    color = vec4(clamp(diffuse + vObjColor.xyz*specular, 0.0, 1.0), 1.0);
}
