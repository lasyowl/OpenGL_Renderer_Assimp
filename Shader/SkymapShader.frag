#version 430 core

layout(location = 0) in vec2 shaderTexCoord;

layout(location = 0) out vec4 finalColor;

uniform sampler2D textureSampler;

void main(){

	finalColor = texture(textureSampler, shaderTexCoord);

}