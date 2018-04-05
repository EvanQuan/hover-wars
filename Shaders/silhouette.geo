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
	vec3 cameraUpVec = normalize( vec3( modelview[0][1], 
									    modelview[1][1], 
									    modelview[2][1] ) );
	

	/*
		Triangle 1
		
		0
		|
		1----------------------2
	*/
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + (fWidth * ObjN[0]), 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz, 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz, 1.0 );
	EmitVertex();
	EndPrimitive();
	
	/*
		Triangle 2
		
		1-----------------------3
		                        |
								2
	*/
	gl_Position = projection * modelview * vec4( gl_in[0].gl_Position.xyz + (fWidth * ObjN[0]), 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz, 1.0 );
	EmitVertex();
	gl_Position = projection * modelview * vec4( gl_in[1].gl_Position.xyz + (fWidth * ObjN[1]), 1.0 );
	EmitVertex();
	EndPrimitive();
}
