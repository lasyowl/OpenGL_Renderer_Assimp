#version 430 core

layout(location = 0) in vec3 shaderVertex_EC;
layout(location = 1) in vec3 shaderNormal_EC;
layout(location = 2) in vec2 shaderTexCoord;
layout(location = 3) in vec3 lightVector;
layout(location = 4) in vec3 posToEyeVector;
layout(location = 5) in float lightToPosDistanceFactor;

layout(location = 0) out vec4 finalColor;

uniform sampler2D textureSampler;

uniform vec4 color_ambient;
uniform vec4 color_diffuse;
uniform vec4 color_specular;

uniform bool useTexture = false;

vec4 shadedColor;
vec4 loadedTexture;

void main(){

	float diffuseFactor = max(dot(lightVector, shaderNormal_EC), 0.0f);
	float distanceFactor = pow(1 / lightToPosDistanceFactor, 3.0f);
	float specularFactor = max(dot(reflect(-lightVector, shaderNormal_EC), posToEyeVector), 0.0f);

	vec4 ambientColor = color_ambient;
	vec4 diffuseColor = diffuseFactor * color_diffuse;
	float reflectivity = 5.0f;
	vec4 specularColor = pow(specularFactor, reflectivity) * color_specular;

	shadedColor = distanceFactor * (specularColor + diffuseColor) + ambientColor;

	if((loadedTexture = texture(textureSampler, shaderTexCoord)) == vec4(0.0, 0.0, 0.0, 1.0)){
		finalColor = shadedColor * 0.8;
	}
	else if(useTexture == false){
		finalColor = shadedColor * 0.5;
	}
	else {
		finalColor = texture(textureSampler, shaderTexCoord);
	}
	finalColor = vec4(1,1,1,1);
}