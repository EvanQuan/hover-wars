// Layouts:
//	vertex - local coordinate of vertex
//	translate - instance vbo, translation matrix.
layout (location = 0) in vec3 vertex;

/*
 * Do not alter the position of anything.
 */
void main(void)
{
    // Just to ensure that the z value is 0 in all cases.
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    // gl_Position = vec4(vertex, 1.0);
}
