
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
};

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 vNormal[];
in vec2 vDimensions[];
in vec3 vToCamera[];
in vec2 vUVStart[];
in vec2 vUVEnd[];
in float fDuration[];

out vec2 TexCoords;

void main(void)
{	
	vec3 vRightVector = normalize( cross( vNormal[0], vToCamera[0]) );
	vRightVector *= (vDimensions[0].y * 0.5);
	vec3 vUpVector = vNormal[0] * vDimensions[0].x;
	
	// Shrink Billboard after a certain duration
	if( fDuration[0] < 1.0f )
	{
		vRightVector *= fDuration[0];
		vUpVector *= fDuration[0];
	}
	
	/* Create a Track:
			P3---------------------------------P4
			|								   |
			|                                  |
			|                                  |
			P1---------------------------------P2
	*/
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz - vRightVector, 1.0 );   /* P1 */
	TexCoords = vec2( vUVStart[0].x, vUVEnd[0].y );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + vRightVector, 1.0 );  /* P2 */
	TexCoords = vec2( vUVEnd[0] );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + vUpVector - vRightVector, 1.0 );   /* P3 */
	TexCoords = vec2( vUVStart[0] );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + vUpVector + vRightVector, 1.0 );  /* P4 */
	TexCoords = vec2( vUVEnd[0].x, vUVStart[0].y );
	EmitVertex();
	EndPrimitive();
}
