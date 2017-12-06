#include "GlobalFunctions.h"


void GlobalFunctions::Start() {
	start = clock();
}

void GlobalFunctions::End(char* msg) {
	end = clock();
	cout << msg << ": ";
	cout << end - start << "ms elapsed" << endl;
	start = 0;
}

int GlobalFunctions::End() {
	end = clock();
	int temp = end - start;
	start = 0;
	return temp;
}

void GlobalFunctions::SetAnimTimestamp() {
	animTimeStamp = clock();
}

int GlobalFunctions::GetAnimTimestamp() {
	return animTimeStamp;
}

void GlobalFunctions::PrintMatrix(glm::mat4 matrix) {
	for (int x = 0; x < 4; x++) {
		cout << "[ ";
		for (int y = 0; y < 4; y++) {
			printf("%.6f ", matrix[x][y]);
		}
		cout << "]" << endl;
	}
	cout << endl;
}

void GlobalFunctions::PrintMatrix(aiMatrix4x4 matrix) {
	for (int x = 0; x < 4; x++) {
		cout << "[ ";
		for (int y = 0; y < 4; y++) {
			printf("%.6f ", matrix[x][y]);
		}
		cout << "]" << endl;
	}
	cout << endl;
}

mat4 GlobalFunctions::AiToGlm(aiMatrix4x4 matrix) {
	mat4 answer(0.0f);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			answer[i][j] = matrix[i][j];
		}
	}
	return answer;
}

bool GlobalFunctions::FpsCounter() {
	if (fpsCount_flag == false) {
		fpsCount_flag = !fpsCount_flag;
		return false;
	}
	else {
		if (1000.0f / (float)(clock() - fpsCount_start) < 60.0f) { // fps Á¶Àý
			//cout << clock() - fpsCount_start << endl;
			fpsCount_start = clock();
			fpsCount_flag = !fpsCount_flag;
			return true;
		}
		else return false;
	}
}