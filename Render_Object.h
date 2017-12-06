#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <freeImage/FreeImage.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Assimp_Loader.h"
#include "Containers.h"
#include "GlobalFunctions.h"
#include "Variables.h"

using namespace std;
using namespace Assimp;

typedef unsigned int uInt;

#define NUMBER_OF_MESH_VBO 6
#define NUMBER_OF_BONE_VBO 1
#define MAX_WEIGHT_BONE_NUMBER 4

class Render_Object {
public:
	Render_Object();
	Render_Object(const char *filePath);

	int LoadFile(const char *filePath);
	void InitScene(GLuint program, Variables *globalVar, bool animate, bool instancing);
	void LoadTransformData(const char *filePath);
	void Draw(Variables *globalVar, bool animateFlag);
	void InstanceDraw(Variables *globalVer, bool animateFlag);

	aiScene *scene;
	vector<GLuint> vao;
	GLuint *vbo;
	GLuint *vao_index;
	GLuint *vao_bone;
	/*vector<GLuint> vbo;
	vector<GLuint> vao_index;
	vector<GLuint> vao_bone;*/
	vector<GLuint> vbo_bone;
	map<string, GLuint> texPathMap;
	vector<MeshData> meshData;
	mat4 GlobalInverseMatrix;
	Variables *var;

private:
	void LoadTexture(const char* filePath);
	void GenRenderObjects();
	void BindRenderData();
	void BindBoneData();
	void GenBoneData();
	void GenBoneMatrix(aiNode *currentNode, mat4 ParentModelTransform);
	void GenTextures();
	void BindWeightArrays(int meshIndex);
	void GenGlobalInverseMatrix();
	void CalcTimeElapsed();
	void StartAnimation();
	void StopAnimation();
	aiAnimation* FindAnimation(aiNode *node);
	aiNodeAnim* FindNodeAnim(aiAnimation *animation, const char *nodeName);
	mat4 CalcRMat(float tick, aiNodeAnim *nodeAnim, float duration);
	mat4 CalcSMat(float tick, aiNodeAnim *nodeAnim, float duration);
	mat4 CalcTMat(float tick, aiNodeAnim *nodeAnim, float duration);
	void SetShaderVariables(int materialIndex, mat4 ModelMatrix, int meshIndex);
	void SetShaderProgram(GLuint value);
	void UpdateVariables(Variables *globalVar);

	Assimp_Loader importer;
	GlobalFunctions gf;
	GLuint shaderProgram;

	int numInstance;
	int boneNumber;
	int timestamp;
	int timeElapsed;
	bool animate;
	bool instancingFlag;
	int currentTime;
	//float currentTick;
};
