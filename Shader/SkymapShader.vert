#version 430 core

layout(location = 0) in vec3 appVertex;
layout(location = 1) in vec3 appNormal;
layout(location = 2) in vec2 appTexCoord;

layout(location = 0) out vec2 shaderTexCoord;

uniform mat4 ModelViewProjectionMatrix;

void main(){

	shaderTexCoord = appTexCoord;
	
	gl_Position = ModelViewProjectionMatrix * vec4(appVertex, 1.0);

}