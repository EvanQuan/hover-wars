// Layouts:
//	vertex - local coordinate of vertex
//	translate - instance vbo, translation matrix.
layout (location = 0) in vec4 vertex;	// <vec2 pos, vec2 tex>

uniform mat4 UIProjection;
out vec2 TexCoords;

/*
 * Do not alter the position of anything.
 */
void main(void)
{
    // Just to ensure that the z value is 0 in all cases.
    gl_Position = UIProjection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
