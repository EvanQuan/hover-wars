
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;		// LightSpaceMatrix
	mat4 modelview;			// Model Matrix
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};

// Layouts:
//	vertex - local coordinate of vertex
//	translate - instance vbo, translation matrix.
layout (location = 0) in vec3 vertex;
layout (location = 3) in mat4 translate;

// Translate Position to World Coordinates then apply modelview and projection transformations
void main(void)
{
    gl_Position = dirLightSpaceMat * translate * vec4(vertex, 1.0);
}
