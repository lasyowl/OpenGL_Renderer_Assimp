#ifndef __GLOBALFUNCTIONS_H
#define __GLOBALFUNCTIONS_H

#pragma once

#include <Windows.h>
#include <time.h>
#include <iostream>

#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>

using namespace std;
using namespace glm;

class GlobalFunctions {
public:
	GlobalFunctions() : start(0), end(0) {}

	void Start();
	void End(char *msg);
	int End();
	void SetAnimTimestamp();
	int GetAnimTimestamp();
	void PrintMatrix(glm::mat4 matrix);
	void PrintMatrix(aiMatrix4x4 matrix);
	mat4 AiToGlm(const aiMatrix4x4 matrix);
	bool FpsCounter();
private:
	int start, end;
	int animTimeStamp;
	int fpsCount_start;
	bool fpsCount_flag;
};

#endif