#version 430 core

uniform mat4 modelview;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 N;
out vec3 P;
out vec3 L;
out vec2 texturePos;

uniform vec3 lightPosition;

void main (void)
{	
    mat3 normalMatrix = mat3(transpose(inverse(modelview)));
    N = normalize(normalMatrix*normal);
    vec4 L4 = modelview * vec4(lightPosition, 1.0);
    vec4 P4 = modelview * vec4( position.x, position.y, position.z, 1.0);
    L = L4.xyz/L4.w;
    P = P4.xyz/P4.w;
	
	texturePos = uv;

    gl_Position = projection *  P4;
}
