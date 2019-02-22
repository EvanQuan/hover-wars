
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
	mat4 dirLightSpaceMat;
	mat4 spotLightSpaceMat[4];
};

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 translation;

out vec3 V;
out vec3 vNormalInCameraSpace;
out vec2 TexCoords;

void main(void)
{
	vec3 vPositionModelSpace = (translation * vec4( vertex, 1.0 )).xyz;
	vec4 vPositionCameraSpace = modelview * vec4(vPositionModelSpace, 1.0);
	vec3 P = vPositionCameraSpace.xyz/vPositionCameraSpace.w;
	
	TexCoords = uv;
	
	// Normal Matrix to correct Normal in camera space
	mat3 m3NormalMatrix = transpose( inverse( mat3( modelview ) ) );
	vNormalInCameraSpace = normalize( m3NormalMatrix * normal );
	V = normalize( -P );
    gl_Position = projection * vPositionCameraSpace;
}
