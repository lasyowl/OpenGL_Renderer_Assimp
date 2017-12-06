#version 430 core

layout(location = 0) in vec3 appVertex;
layout(location = 1) in vec3 appNormal;
layout(location = 2) in vec2 appTexCoord;

layout(location = 0) out vec3 v_Vertex;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main(){

	gl_Position = vec4(appVertex, 1.0f);

	v_Normal = appNormal;

	v_TexCoord = appTexCoord;

}