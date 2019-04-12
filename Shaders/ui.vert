
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};

// Layouts:
//	vertex - local coordinate of vertex
//	translate - instance vbo, translation matrix.
layout (location = 0) in vec4 vertex;	// <vec2 pos, vec2 tex>

uniform mat4 UIProjection;
uniform bool backgroundImage;
out vec2 TexCoords;

/**
 * User Interface Shader.
 *
 * Used for all the text and images of the user interface.
 * Since the UI should always be visible as the camera moves,
 * we do not want to alter the position of anything.
 * This maintains all the vertices unaltered in screen space.
 */
void main(void)
{
    // Just to ensure that the z value is 0 in all cases.
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
	
	if (!backgroundImage)
	    gl_Position = UIProjection * gl_Position;


    TexCoords = vertex.zw;
}
