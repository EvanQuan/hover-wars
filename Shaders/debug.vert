
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
layout (location = 0) in vec3 vertex;
layout (location = 1) in mat4 translate;

// Translate Position to World Coordinates then apply modelview and projection transformations
void main(void)
{
    gl_Position = projection * modelview * translate * vec4(vertex, 1.0);
}
