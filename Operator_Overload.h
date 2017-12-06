#ifndef __OPERATOR_OVERLOAD_H
#define __OPERATOR_OVERLOAD_H


#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>

// * needs adjustment : col major multiplication
glm::mat4 operator*(glm::mat4 Ref, aiMatrix4x4 Mat) {
	glm::mat4 answer(0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				answer[i][j] += Ref[i][k] * Mat[k][j];
			}
		}
	}
	return answer;
}

// * needs adjustment : col major multiplication
glm::mat4 operator*(aiMatrix4x4 Ref, glm::mat4 Mat) {
	glm::mat4 answer(0);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				answer[i][j] += Ref[i][k] * Mat[k][j];
			}
		}
	}
	return answer;
}



glm::vec3 operator*(float num, glm::vec3 vec) {
	vec.x *= num;
	vec.y *= num;
	vec.z *= num;
	return vec;
}

#endif