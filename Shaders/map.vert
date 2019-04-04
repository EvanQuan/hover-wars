
// xyz = position
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};

out float bDrawTri;
out vec3 vColor;

void main(void)
{
	// Transport To Screen Space to evaluate frustum culling.
	vec4 vScreenSpaceCoords = (projection * modelview * vec4(vertex, 1.0));
	vColor = color;
	
	// Check to see if vertex is within view frustum.
	bool bInX = (vScreenSpaceCoords.x < vScreenSpaceCoords.w) && 
				(vScreenSpaceCoords.x > -vScreenSpaceCoords.w);
	bool bInY = (vScreenSpaceCoords.y < vScreenSpaceCoords.w) && 
				(vScreenSpaceCoords.y > -vScreenSpaceCoords.w);
	bool bInZ = (vScreenSpaceCoords.z < vScreenSpaceCoords.w) && 
				(vScreenSpaceCoords.z > -vScreenSpaceCoords.w);
	
	// Send Boolean to determine if within frustum.
	bDrawTri = 	(bInX && bInY && bInZ) ? 0.0 : 1.0;
				
	// Output result and cap to edges of screen.
    gl_Position = (vScreenSpaceCoords / vScreenSpaceCoords.w);   

	gl_Position = vec4(clamp(gl_Position.x, -1.0, 1.0),
					   clamp(gl_Position.y, -1.0, 1.0), 
					   -clamp(gl_Position.z, -1.0, 1.0), 1.0);
	if(!bInZ)
	{
		gl_Position.y = gl_Position.z;
		gl_Position.x = -gl_Position.x;
	}
}
