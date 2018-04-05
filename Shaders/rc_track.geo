#version 430 core

layout (lines) in;
layout (points, max_vertices = 4) out;

uniform mat4 modelview;
uniform mat4 projection;

uniform float fWidth = 0.1;
uniform vec3 v3_Gravity = vec3( 0.0, -9.81, 0.0 );

in vec3 BiN[];

void main(void)
{	
	float fHalfWidth = fWidth * 0.5;
	vec3 vTangent = normalize( gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz );
	vec3 BiNormal[2] = { cross( vTangent, BiN[0] ),
						 cross( vTangent, BiN[1] ) };
	
	/* Create a Track:
			P1---------------------------------P4
			|								   |
			|                                  |
			|                                  |
			P2---------------------------------P3
	*/
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + ( fHalfWidth * BiNormal[0] ), 1.0 );   /* P1 */
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz - ( fHalfWidth * BiNormal[0] ), 1.0 );  /* P2 */
	EmitVertex();
	EndPrimitive();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz + ( fHalfWidth * BiNormal[1] ), 1.0 );   /* P1 */
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz - ( fHalfWidth * BiNormal[1] ), 1.0 );  /* P2 */
	EmitVertex();
	EndPrimitive();
}
