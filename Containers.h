#ifndef __CONTAINERS_H
#define __CONTAINERS_H

#pragma once
#include <vector>

#include <glm/glm.hpp>

using namespace std;

struct MeshData {
	vector<glm::mat4> OffsetBoneMatrix;
	vector<glm::mat4> GlobalBoneMatrix;
	map<string, GLuint> boneMap;
};

#endif