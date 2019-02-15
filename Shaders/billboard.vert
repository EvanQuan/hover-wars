
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
	mat4 modelviewInv;
};

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvStart;
layout (location = 3) in vec2 uvEnd;
layout (location = 4) in vec2 Dimensions;
layout (location = 5) in float Duration;

out vec3 vNormal;
out vec2 vDimensions;
out vec3 vToCamera;
out vec2 vUVStart;
out vec2 vUVEnd;
out float fDuration;

void main(void)
{
	// Get Position in Camera Space to get ToCamera vector
    vec4 positionCameraSpace = vec4( modelview[0][2], modelview[1][2], modelview[2][2], modelview[3][2] );
	vec3 P = positionCameraSpace.xyz * positionCameraSpace.w;
	
	vToCamera = vec3( modelviewInv[3][0], modelviewInv[3][1], modelviewInv[3][2]) - vertex;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	vNormal = normal;
	
	// Send Duration to the geometry shader.
	fDuration = Duration;
	
	// Pass Dimensions to Geometry Shader
	vDimensions = Dimensions;
	vUVStart = uvStart;
	vUVEnd = uvEnd;
	
	// Pass Fragment Position.
    gl_Position = vec4( vertex, 1.0 );
}
