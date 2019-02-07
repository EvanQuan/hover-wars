
layout (std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 modelview;
};

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvStart;
layout (location = 3) in vec2 uvEnd;
layout (location = 4) in vec2 Dimensions;

out vec3 vNormal;
out vec2 vDimensions;
out vec3 vToCamera;
out vec2 vUVStart;
out vec2 vUVEnd;

void main(void)
{
	// Get Position in Camera Space to get ToCamera vector
    vec4 positionCameraSpace = vec4( modelview[0][2], modelview[1][2], modelview[2][2], modelview[3][2] );
	vec3 P = positionCameraSpace.xyz * positionCameraSpace.w;
	
	mat4 CameraMatrix = inverse(modelview);
	
	vToCamera = vec3( CameraMatrix[3][0], CameraMatrix[3][1], CameraMatrix[3][2]) - vertex;
	
	// create the Normal Matrix to correct Normal into camera space
	mat3 normalMatrix = transpose(inverse(mat3(modelview)));
	vNormal = normal;
	
	// Pass Dimensions to Geometry Shader
	vDimensions = Dimensions;
	vUVStart = uvStart;
	vUVEnd = uvEnd;
	
	// Pass Fragment Position.
    gl_Position = vec4( vertex, 1.0 );
}
