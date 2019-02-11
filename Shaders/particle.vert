
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
};

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 uv;

out vec2 TexCoords;

void main(void)
{
	TexCoords = uv;
	
    gl_Position = projection * modelview * vec4(vertex, 1.0);
}
