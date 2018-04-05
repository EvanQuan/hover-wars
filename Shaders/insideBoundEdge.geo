#version 430 core

layout (lines) in;
layout (triangle_strip, max_vertices = 6) out;

uniform mat4 modelview;
uniform mat4 projection;

uniform float fWidth;

in vec3 V[];
in vec3 N[];

in vec3 ObjN[];

void main(void)
{	
	vec3 cameraLookAtVec = normalize( vec3( modelview[0][2], 
									        modelview[1][2], 
											modelview[2][2] ) );
	vec3 widthDirection = normalize( cross( normalize(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz), 
											cameraLookAtVec ) );
											
	float fHalfWidth = fWidth / 2.0;
	

	/*
		Triangle 1
		
		1
		|
		0----------------------2
	*/
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + (-fHalfWidth * widthDirection), 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + (fHalfWidth * widthDirection), 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz + (-fHalfWidth * widthDirection), 1.0 );
	EmitVertex();
	EndPrimitive();
	
	/*
		Triangle 2
		
		2-----------------------3
		                        |
								1
	*/
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz + (-fHalfWidth * widthDirection), 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + (fHalfWidth * widthDirection), 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz + (fHalfWidth * widthDirection), 1.0 );
	EmitVertex();
	EndPrimitive();
}
