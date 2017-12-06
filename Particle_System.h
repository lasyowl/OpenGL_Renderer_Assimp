#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Variables.h"

#define MAX_PARTICLE_NUM 10000
#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f

typedef unsigned int uInt;

struct Particle {
	float type;
	glm::vec3 position;
	glm::vec3 velocity;
	float timeRemain;
};

class Particle_System {
public:
	Particle_System();

	void Initiate(glm::vec3 pos, GLuint updateProgram, GLuint renderProgram);
	void Render(int deltaTime, Variables *variables);

private:
	void UpdateParticles(int deltaTime);
	void RenderParticles(glm::mat4 ViewProjectionMatrix, glm::vec3 eyePosition);
	void SetUpdateShaderVariables();
	void SetRenderShaderVariables();

	GLuint uProgram, rProgram;
	GLuint transformFeedback[2], particleBuffer[2];
	std::vector<Particle> particles;
	int currentTime;
	bool isFirst;
	GLuint currentVB, currentTFB;
	Variables *var;
};