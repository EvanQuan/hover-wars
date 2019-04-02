
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};


layout (location = 0) in vec3 vertex;

out vec3 vTextureDir;

void main(void)
{
	vTextureDir = vertex;
	mat4 noTranslationMV = mat4(mat3(modelview));
    gl_Position = projection * noTranslationMV * vec4(vertex, 1.0);
	gl_Position = gl_Position.xyww;
}
