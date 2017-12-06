#version 430 core

layout(location = 0) in vec3 shaderVertex_EC;
layout(location = 1) in vec3 shaderNormal_EC;
layout(location = 2) in vec2 shaderTexCoord;
layout(location = 3) in vec2 alphamapCoord;
layout(location = 4) in vec3 lightVector;
layout(location = 5) in vec3 posToEyeVector;
layout(location = 6) in float posToEyeDistance;
layout(location = 7) in float lightToPosDistanceFactor;

layout(location = 0) out vec4 finalColor;

uniform sampler2DArray terrainTexture;
uniform sampler2DArray heightmapArray;

uniform vec4 color_ambient;
uniform vec4 color_diffuse;
uniform vec4 color_specular;

uniform bool useTexture = true;

vec4 shadedColor;
vec4 loadedTexture;

void main(){

	float diffuseFactor = max(dot(lightVector, shaderNormal_EC), 0.0f);
	float distanceFactor = pow(1 / lightToPosDistanceFactor, 3.0f);
	float specularFactor = max(dot(reflect(-lightVector, shaderNormal_EC), posToEyeVector), 0.0f);

	vec3 ambientColor = vec3(0.5f, 0.5f, 0.5f);
	vec3 diffuseColor = diffuseFactor * vec3(1.0f, 1.0f, 1.0f);
	float reflectivity = 5.0f;
	vec3 specularColor = pow(specularFactor, reflectivity) * vec3(1.0f, 1.0f, 1.0f);

	shadedColor = vec4(distanceFactor * (specularColor + diffuseColor) + ambientColor, 1.0);

	vec4 alphamap = vec4(1.0f, 1.0f, 1.0f, 1.0f) - texture(heightmapArray, vec3(alphamapCoord, 1.0f));
	vec4 textureColor = (alphamap.r * texture(terrainTexture, vec3(shaderTexCoord, 5.0f))) + (alphamap.g * texture(terrainTexture, vec3(shaderTexCoord, 8.0f)));

	if(textureColor == vec4(0.0, 0.0, 0.0, 0.0)){
		finalColor = shadedColor * 0.8;
	}
	else if(useTexture == false){
		finalColor = shadedColor * 0.5;
	}
	else {
		finalColor = textureColor * shadedColor;
	}
}