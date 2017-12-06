#ifndef __ASSIMP_LOADER_H
#define __ASSIMP_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

typedef unsigned int uint;

class Assimp_Loader {
public:
	Assimp_Loader();
	
	aiScene* LoadScene(const char *filePath, int flag);

	aiScene *rootScene;

private:
	void InitScene(const aiScene *tempScene);
	void LoadAnimation(const aiScene *importer);
	void LoadMaterial(const aiScene *importer);
	void LoadMesh(const aiScene *importer);
	void LoadTexture(const aiScene *tempScene);
	void LoadNode(aiNode* node, aiNode* tempNode);

	aiNodeAnim* LoadNodeAnim(aiNodeAnim *tempNodeAnim);
	aiBone* LoadBone(aiBone *tempBone);
};
#endif