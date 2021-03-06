
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};


layout (location = 0) in vec3 vertex;

out vec3 vVert;

void main(void)
{
	vVert = vertex;
    gl_Position = projection * modelview * vec4(vertex, 1.0);
}
