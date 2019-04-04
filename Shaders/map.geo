
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float bDrawTri[];
in vec3 vColor[];

out vec4 vFragColor;

// Constants
float f45Deg_Rad 		= 0.785398;
float fSideLength 		= 0.05;
float fMidLength 		= 0.025;
vec3 OverheadDir		= vec3( 0.0, 1.0, 0.0 );
float fOverheadOffset	= 0.2;

// Pulled from https://bit.ly/2Vmyqi3
// Create a Quaternion from a given axis and angle
vec4 quat_from_axis_angle(vec3 axis, float angle)
{ 
  vec4 qr;
  float half_angle = (angle * 0.5);
  qr.x = axis.x * sin(half_angle);
  qr.y = axis.y * sin(half_angle);
  qr.z = axis.z * sin(half_angle);
  qr.w = cos(half_angle);
  return qr;
}

// Rotate a vertex to a given angle on a given axis
vec3 rotate_vertex_position(vec3 vPos, vec3 vAxis, float fAngle )
{
	vec4 q = quat_from_axis_angle(vAxis, fAngle);
	vec3 v = vPos.xyz;
	return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main(void)
{	
	// Rotation Matrix 2D
	mat2 m2Rotation = mat2(cos(f45Deg_Rad), -sin(f45Deg_Rad),
						   sin(f45Deg_Rad), cos(f45Deg_Rad));
							   
	if( 1.0 == bDrawTri[0] )
	{
		vec2 vToCenter = normalize(vec2(0.0) - gl_in[0].gl_Position.xy);
		
		/* Create a Track:
				4
				|\
				| \
				3--2
				| /
				|/
				1
		*/
		gl_Position = vec4( gl_in[0].gl_Position.xy + (fSideLength * transpose(m2Rotation) * vToCenter.xy), gl_in[0].gl_Position.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
		
		gl_Position = vec4( gl_in[0].gl_Position.xy, gl_in[0].gl_Position.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
		
		gl_Position = vec4( gl_in[0].gl_Position.xy + (fMidLength * vToCenter.xy), gl_in[0].gl_Position.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
	
		gl_Position = vec4(gl_in[0].gl_Position.xy + (fSideLength * m2Rotation * vToCenter.xy), gl_in[0].gl_Position.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
		EndPrimitive();
	}
	else
	{
		// Move position to above vehicle
		vec3 vPosition = gl_in[0].gl_Position.xyz + (fOverheadOffset * OverheadDir);
		vec3 vAxis = vec3( 0.0, 0.0, 1.0 );
		float fScale = length(vAxis);
		
		// Get Left and Right Sides of triangle
		/* Create a Tri:
			1--3--4
			 \ | /
			  \|/
			   2
		*/
		gl_Position = vec4( vPosition.xy + (fSideLength * transpose(m2Rotation) * OverheadDir.xy), vPosition.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
		
		gl_Position = vec4( vPosition.xy, vPosition.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
		
		gl_Position = vec4( vPosition.xy + (fMidLength * OverheadDir.xy), vPosition.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
	
		gl_Position = vec4(vPosition.xy + (fSideLength * m2Rotation * OverheadDir.xy), vPosition.z, 1.0);
		vFragColor = vec4( vColor[0], 1.0 );
		EmitVertex();
		EndPrimitive();
	}
}
