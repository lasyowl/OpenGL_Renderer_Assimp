#version 430 core

layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) out vec3 shaderVertex_EC;
layout(location = 1) out vec3 shaderNormal_EC;
layout(location = 2) out vec2 shaderTexCoord;
layout(location = 3) out vec2 alphamapCoord;
layout(location = 4) out vec3 lightVector;
layout(location = 5) out vec3 posToEyeVector;
layout(location = 6) out float posToEyeDistance;
layout(location = 7) out float lightToPosDistanceFactor;

layout(invocations = 9, triangles) in;

layout(location = 0) in vec3 v_Vertex[];
layout(location = 1) in vec3 v_Normal[];
layout(location = 2) in vec2 v_TexCoord[];

uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ViewProjectionMatrix;
uniform mat3 ModelViewInverseMatrix;

uniform vec3 sunPosition;
uniform vec3 eyePosition;
uniform float heightmapSize;
uniform vec2 heightmapTextureSize;

uniform sampler2DArray heightmapArray;

void main(void) {

	vec4 pos[3];
	vec2 alpha[3];
	vec3 norm[3];

	for (int i = 0; i < gl_in.length(); i++) {

		// * Generates 4 planes(heightmapSize)
		vec2 heightmapScale;
		heightmapScale.x = heightmapTextureSize.x / heightmapSize; 
		heightmapScale.y = heightmapTextureSize.y / heightmapSize; 

		vec2 heightmapCoord;
		heightmapCoord.x = gl_in[i].gl_Position[0] / heightmapTextureSize.x + (gl_InvocationID % 3) / heightmapScale.x;
		heightmapCoord.y = gl_in[i].gl_Position[1] / heightmapTextureSize.y + (gl_InvocationID / 3) / heightmapScale.y;
		vec2 heightmapCoordL;
		heightmapCoordL.x = (gl_in[i].gl_Position[0] - 1.0) / heightmapTextureSize.x + (gl_InvocationID % 3) / heightmapScale.x;
		heightmapCoordL.y = gl_in[i].gl_Position[1] / heightmapTextureSize.y + (gl_InvocationID / 3) / heightmapScale.y;
		vec2 heightmapCoordR;
		heightmapCoordR.x = (gl_in[i].gl_Position[0] + 1.0) / heightmapTextureSize.x + (gl_InvocationID % 3) / heightmapScale.x;
		heightmapCoordR.y = gl_in[i].gl_Position[1] / heightmapTextureSize.y + (gl_InvocationID / 3) / heightmapScale.y;
		vec2 heightmapCoordU;
		heightmapCoordU.x = gl_in[i].gl_Position[0] / heightmapTextureSize.x + (gl_InvocationID % 3) / heightmapScale.x;
		heightmapCoordU.y = (gl_in[i].gl_Position[1] + 1.0) / heightmapTextureSize.y + (gl_InvocationID / 3) / heightmapScale.y;
		vec2 heightmapCoordD;
		heightmapCoordD.x = gl_in[i].gl_Position[0] / heightmapTextureSize.x + (gl_InvocationID % 3) / heightmapScale.x;
		heightmapCoordD.y = (gl_in[i].gl_Position[1] - 1.0) / heightmapTextureSize.y + (gl_InvocationID / 3) / heightmapScale.y;

		vec4 height = texture(heightmapArray, vec3(heightmapCoord, 0.0f));
		vec4 heightL = texture(heightmapArray, vec3(heightmapCoordL, 0.0f));
		vec4 heightR = texture(heightmapArray, vec3(heightmapCoordR, 0.0f));
		vec4 heightU = texture(heightmapArray, vec3(heightmapCoordU, 0.0f));
		vec4 heightD = texture(heightmapArray, vec3(heightmapCoordD, 0.0f));

		norm[i] = normalize(vec3(heightL.x - heightR.x, 4.0f / heightmapSize, heightU.x - heightD.x));
		alpha[i] = heightmapCoord;

		pos[i] = gl_in[i].gl_Position
					+ vec4(heightmapSize * (gl_InvocationID % 3), heightmapSize * (gl_InvocationID / 3), 60 * height.x - 5, 0.0f) 
					- vec4(heightmapSize * 1.5f, heightmapSize * 1.5f, 0.0f, 0.0f);
	}
	
	for (int i = 0; i < gl_in.length(); i++) {
		
		shaderTexCoord = gl_in[i].gl_Position.xy;// / heightmapSize * 5.0f;
		alphamapCoord = alpha[i];
		lightVector = normalize(sunPosition - vec3(ModelMatrix * pos[i]));
		lightToPosDistanceFactor = distance(sunPosition, vec3(ModelMatrix * pos[i])) / 20000;
		posToEyeVector = normalize(eyePosition - vec3(ModelMatrix * pos[i]));
		
		shaderNormal_EC = norm[i];
		
		gl_Position = ModelViewProjectionMatrix * pos[i];

		EmitVertex();
	}
	
	EndPrimitive();
}