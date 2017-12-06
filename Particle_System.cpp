#include "Particle_System.h"

Particle_System::Particle_System() :currentTime(0), isFirst(true) {

}

void Particle_System::Initiate(glm::vec3 pos, GLuint updateProgram, GLuint renderProgram) {
	uProgram = updateProgram;
	rProgram = renderProgram;

	particles.resize(MAX_PARTICLE_NUM);

	particles[0].type = PARTICLE_TYPE_LAUNCHER;
	particles[0].position = pos;
	particles[0].velocity = glm::vec3(0.0f, 2.0f, 0.0f);
	particles[0].timeRemain = 0.0f;

	glGenTransformFeedbacks(2, transformFeedback);
	glGenBuffers(2, particleBuffer);

	for (int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles.size(), &particles[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[i]);
	}
}

void Particle_System::Render(int deltaTime, Variables *var) {
	currentTime += deltaTime;
	
	UpdateParticles(deltaTime);
	RenderParticles(var->ViewProjectionMatrix, var->eyePosition);

	currentVB = currentTFB;
	currentTFB = (currentTFB + 1) & 0x1;
	Particle pp[100];
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currentVB]);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, 100 * sizeof(Particle), pp);
	
	for (int i = 0; i < 100; i++) {
		if (pp[i].type > 0)
			printf("%d %f\n", i, pp[i].type);
	}
	glUseProgram(0);
}

void Particle_System::UpdateParticles(int deltaTime) {

	glUseProgram(uProgram);
	SetUpdateShaderVariables();

	glEnable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currentVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[currentTFB]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)0); // type
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)4); // position
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)16);// velocity
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)28);// time remain

	glBeginTransformFeedback(GL_POINTS);

	if (isFirst == true) {
		glDrawArrays(GL_POINTS, 0, 1);
		isFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, transformFeedback[currentVB]);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void Particle_System::RenderParticles(glm::mat4 ViewProjectionMatrix, glm::vec3 eyePosition) {
	glUseProgram(rProgram);

	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currentTFB]);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)4); // position

	glDrawTransformFeedback(GL_POINTS, transformFeedback[currentTFB]);

	glDisableVertexAttribArray(0);
}

void Particle_System::SetUpdateShaderVariables() {
	
}

void Particle_System::SetRenderShaderVariables() {
	glUniformMatrix4fv(var->loc_VP, 1, GL_FALSE, &var->ViewProjectionMatrix[0][0]);
}