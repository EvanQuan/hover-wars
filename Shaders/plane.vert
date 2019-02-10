
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 translate;

out vec3 NormalVector;
out vec3 vFragPosition;
out vec3 ToCamera;
out vec2 TexCoords;

void main (void)
{	
    mat3 normalMatrix = mat3(transpose(inverse(modelview)));
    NormalVector = normalize(normalMatrix*normal);
	
	vec3 translatedPosition = ( translate * vec4( position, 1.0) ).xyz;
	vec4 positionModelSpace = modelview * vec4( translatedPosition, 1.0);
    vFragPosition = positionModelSpace.xyz/positionModelSpace.w;

	ToCamera = normalize( -vFragPosition );
	
	TexCoords = uv;
	
    gl_Position = projection *  positionModelSpace;
}
