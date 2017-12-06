#version 430 core

layout(location = 0) in vec3 appVertex;
layout(location = 1) in vec3 appNormal;
layout(location = 2) in vec2 appTexCoord2;
layout(location = 3) in vec4 weight;
layout(location = 4) in uvec4 weightIndex;
layout(location = 5) in mat4 transformMatrix;

layout(location = 0) out vec3 shaderVertex_EC;
layout(location = 1) out vec3 shaderNormal_EC;
layout(location = 2) out vec2 shaderTexCoord;
layout(location = 3) out vec3 lightVector;
layout(location = 4) out vec3 posToEyeVector;
layout(location = 5) out float lightToPosDistanceFactor;

uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ViewProjectionMatrix;
uniform mat3 ModelViewInverseMatrix;
uniform mat4 BoneMatrix[100];

uniform bool useBone = false;
uniform bool instanceMode;
uniform vec3 sunPosition;
uniform vec3 eyePosition;

void main(){

	shaderVertex_EC = vec3(ModelViewMatrix * vec4(appVertex, 1.0f));

	shaderTexCoord = appTexCoord2;
	
	mat4 WeightMatrix = weight.x * BoneMatrix[weightIndex.x] + weight.y * BoneMatrix[weightIndex.y];

	vec4 newNormal = WeightMatrix * vec4(appNormal, 0.0f);

	vec4 modelPosition;

	if(useBone == true) {
		shaderNormal_EC = vec3(ModelMatrix * newNormal);
		modelPosition = WeightMatrix * vec4(appVertex, 1.0f);
		gl_Position = ViewProjectionMatrix * modelPosition;
	}
	else {
		shaderNormal_EC = vec3(ModelMatrix * vec4(appNormal, 0.0f));
		if(instanceMode == true) {
			modelPosition = transformMatrix * ModelMatrix * vec4(appVertex, 1.0f);
			gl_Position = ViewProjectionMatrix * modelPosition;
		}
		else {
			modelPosition = ModelMatrix * vec4(appVertex, 1.0f);
			gl_Position = ViewProjectionMatrix * modelPosition;
		}
	}
	lightVector = normalize(sunPosition - vec3(modelPosition));
	posToEyeVector = normalize(eyePosition - vec3(modelPosition));
	lightToPosDistanceFactor = distance(sunPosition, vec3(ModelMatrix * modelPosition)) / 20000;
}