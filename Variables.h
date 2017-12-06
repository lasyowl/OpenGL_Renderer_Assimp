#ifndef __VARIABLES_H
#define __VARIABLES_H

#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Variables {
public:
	Variables();

	void SetScaleFactor(float S);
	void SetTransFactor(vec3 T);

	GLfloat backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat testColor[4] = { 1, 1, 1, 1 };

	GLuint loc_M;
	GLuint loc_MV;
	GLuint loc_MVP;
	GLuint loc_VP;
	GLuint loc_MVI;
	GLuint loc_BM;

	GLuint loc_tex;
	GLuint useTexture;
	GLboolean useBone;
	GLboolean loc_instanceMode;
	GLuint loc_ambient;
	GLuint loc_diffuse;
	GLuint loc_specular;
	GLuint loc_heightmapSize;
	GLuint loc_heightmapArray;
	GLuint loc_heightmapTextureSize;
	GLuint loc_terrainTexture;
	GLuint loc_sunPosition;
	GLuint loc_eyePosition;

	vec3 RotAxis;
	mat4 UnitMatrix;
	mat4 ModelMatrix;
	mat4 EyeMatrix;
	mat4 FixedEyeMatrix;
	mat4 ModelViewMatrix;
	mat4 ProjectionMatrix;
	mat4 ModelViewProjectionMatrix;
	mat4 ViewProjectionMatrix;
	mat3 ModelViewInverseMatrix;
	mat4 ScaleMatrix;
	mat4 RotMatrix;
	mat4 TransMatrix;

	vec3 zeroVector = { 0.0f, 0.0f, 0.0f };
	vec3 upVector = { 0.0f, 1.0f, 0.0f };
	vec3 downVector = { 0.0f, -1.0f, 0.0f };
	vec3 leftVector = { -1.0f, 0.0f, 0.0f };
	vec3 rightVector = { 1.0f, 0.0f, 0.0f };
	vec3 frontVector = { 0.0f, 0.0f, 1.0f };
	vec3 backVector = { 0.0f, 0.0f, -1.0f };
	vec3 xAxisVector = { 1.0f, 0.0f, 0.0f };
	vec3 yAxisVector = { 0.0f, 1.0f, 0.0f };
	vec3 zAxisVector = { 0.0f, 0.0f, 1.0f };

	vec3 eyePosition = { 0.0f, 0.0f, 0.0f };
	vec3 eyeDirection = { 0.0f, 0.0f, -1.0f };
	GLfloat sunPosition[3];
	vec3 eyeUpperVector;
	mat4 EyeRotMatrix;

	bool animate = false;
	vec2 mousePosition;
	float scaleFactor;

private:
};
#endif