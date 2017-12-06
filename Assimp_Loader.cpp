#include "Assimp_Loader.h"

Assimp_Loader::Assimp_Loader() {

}

aiScene* Assimp_Loader::LoadScene(const char *filePath, int flag) {
	Assimp::Importer importer;
	importer.ReadFile(filePath, flag);

	const aiScene *tempScene = importer.GetOrphanedScene();
	rootScene = new aiScene;

	InitScene(tempScene);
	LoadAnimation(tempScene);
	LoadMaterial(tempScene);
	LoadMesh(tempScene);
	LoadTexture(tempScene); // Not activated
	LoadNode(rootScene->mRootNode, tempScene->mRootNode);
	importer.FreeScene();
	return rootScene;
}

void Assimp_Loader::InitScene(const aiScene *tempScene) {
	rootScene->mNumAnimations = tempScene->mNumAnimations;
	rootScene->mNumMaterials = tempScene->mNumMaterials;
	rootScene->mNumMeshes = tempScene->mNumMeshes;
	rootScene->mNumTextures = tempScene->mNumTextures;
	rootScene->mRootNode = new aiNode;
	
	rootScene->mRootNode->mName = tempScene->mRootNode->mName;
	rootScene->mRootNode->mNumChildren = tempScene->mRootNode->mNumChildren;
	rootScene->mRootNode->mNumMeshes = tempScene->mRootNode->mNumMeshes;
	rootScene->mRootNode->mTransformation = tempScene->mRootNode->mTransformation;
	rootScene->mRootNode->mMeshes = new uint[rootScene->mRootNode->mNumMeshes];
	for (int i = 0; i < rootScene->mRootNode->mNumMeshes; i++)
		rootScene->mRootNode->mMeshes[i] = tempScene->mRootNode->mMeshes[i];
	rootScene->mRootNode->mChildren = new aiNode*[rootScene->mRootNode->mNumChildren];
	for (int i = 0; i < rootScene->mRootNode->mNumChildren; i++) 
		rootScene->mRootNode->mChildren[i] = new aiNode;
}

void Assimp_Loader::LoadAnimation(const aiScene *tempScene) {
	rootScene->mAnimations = new aiAnimation*[rootScene->mNumAnimations];
	for (int i = 0; i < rootScene->mNumAnimations; i++) {
		rootScene->mAnimations[i] = new aiAnimation;
		rootScene->mAnimations[i]->mNumChannels = tempScene->mAnimations[i]->mNumChannels;
		rootScene->mAnimations[i]->mTicksPerSecond = tempScene->mAnimations[i]->mTicksPerSecond;
		rootScene->mAnimations[i]->mDuration = tempScene->mAnimations[i]->mDuration;
		rootScene->mAnimations[i]->mName = tempScene->mAnimations[i]->mName;

		rootScene->mAnimations[i]->mChannels = new aiNodeAnim*[rootScene->mAnimations[i]->mNumChannels];
		for(int j = 0; j < rootScene->mAnimations[i]->mNumChannels;j++)
			rootScene->mAnimations[i]->mChannels[j] = LoadNodeAnim(tempScene->mAnimations[i]->mChannels[j]);
	}
}

aiNodeAnim* Assimp_Loader::LoadNodeAnim(aiNodeAnim *tempNodeAnim) {
	aiNodeAnim *nodeAnim = new aiNodeAnim;

	nodeAnim->mNodeName = tempNodeAnim->mNodeName;
	nodeAnim->mNumPositionKeys = tempNodeAnim->mNumPositionKeys;
	nodeAnim->mNumRotationKeys = tempNodeAnim->mNumRotationKeys;
	nodeAnim->mNumScalingKeys = tempNodeAnim->mNumScalingKeys;

	nodeAnim->mPositionKeys = new aiVectorKey[nodeAnim->mNumPositionKeys];
	nodeAnim->mRotationKeys = new aiQuatKey[nodeAnim->mNumRotationKeys];
	nodeAnim->mScalingKeys = new aiVectorKey[nodeAnim->mNumScalingKeys];

	for (int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
		nodeAnim->mPositionKeys[i].mTime = tempNodeAnim->mPositionKeys[i].mTime;
		nodeAnim->mPositionKeys[i].mValue = tempNodeAnim->mPositionKeys[i].mValue;
	}
	for (int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
		nodeAnim->mRotationKeys[i].mTime = tempNodeAnim->mRotationKeys[i].mTime;
		nodeAnim->mRotationKeys[i].mValue = tempNodeAnim->mRotationKeys[i].mValue;
	}
	for (int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
		nodeAnim->mScalingKeys[i].mTime = tempNodeAnim->mScalingKeys[i].mTime;
		nodeAnim->mScalingKeys[i].mValue = tempNodeAnim->mScalingKeys[i].mValue;
	}
	return nodeAnim;
}

void Assimp_Loader::LoadMaterial(const aiScene *tempScene) {
	rootScene->mMaterials = new aiMaterial*[rootScene->mNumMaterials];

	for (int i = 0; i < rootScene->mNumMaterials; i++) {
		/*aiString texPath;
		tempScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
		printf("%s\n", texPath);*/
		rootScene->mMaterials[i] = new aiMaterial;
		aiMaterial::CopyPropertyList(rootScene->mMaterials[i], tempScene->mMaterials[i]);
	}
}

void Assimp_Loader::LoadMesh(const aiScene *tempScene) {
	rootScene->mMeshes = new aiMesh*[rootScene->mNumMeshes];

	for (int i = 0; i < rootScene->mNumMeshes; i++) {
		rootScene->mMeshes[i] = new aiMesh;
		rootScene->mMeshes[i]->mMaterialIndex = tempScene->mMeshes[i]->mMaterialIndex;
		rootScene->mMeshes[i]->mName = tempScene->mMeshes[i]->mName;
		rootScene->mMeshes[i]->mNumBones = tempScene->mMeshes[i]->mNumBones;
		for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++) {
			rootScene->mMeshes[i]->mNumUVComponents[j] = tempScene->mMeshes[i]->mNumUVComponents[j];
		}
		rootScene->mMeshes[i]->mNumVertices = tempScene->mMeshes[i]->mNumVertices;

		rootScene->mMeshes[i]->mBones = new aiBone*[rootScene->mMeshes[i]->mNumBones];
		for (int j = 0; j < rootScene->mMeshes[i]->mNumBones; j++)
			rootScene->mMeshes[i]->mBones[j] = LoadBone(tempScene->mMeshes[i]->mBones[j]);

		rootScene->mMeshes[i]->mVertices = new aiVector3D[rootScene->mMeshes[i]->mNumVertices];
		rootScene->mMeshes[i]->mNormals = new aiVector3D[rootScene->mMeshes[i]->mNumVertices];
		for (int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
			rootScene->mMeshes[i]->mTextureCoords[j] = new aiVector3D[rootScene->mMeshes[i]->mNumVertices];

		for (int j = 0; j < rootScene->mMeshes[i]->mNumVertices; j++) {
			rootScene->mMeshes[i]->mVertices[j] = tempScene->mMeshes[i]->mVertices[j];
			rootScene->mMeshes[i]->mNormals[j] = tempScene->mMeshes[i]->mNormals[j];
			for (int k = 0; k < AI_MAX_NUMBER_OF_TEXTURECOORDS; k++) {
				if (tempScene->mMeshes[i]->mTextureCoords[k] == NULL)
					break;
				rootScene->mMeshes[i]->mTextureCoords[k][j] = tempScene->mMeshes[i]->mTextureCoords[k][j];
			}
		}
	}
}

aiBone* Assimp_Loader::LoadBone(aiBone *tempBone) {
	aiBone *bone = new aiBone;

	bone->mName = tempBone->mName;
	bone->mNumWeights = tempBone->mNumWeights;
	bone->mOffsetMatrix = tempBone->mOffsetMatrix;

	bone->mWeights = new aiVertexWeight[bone->mNumWeights];
	for (int i = 0; i < bone->mNumWeights; i++) {
		bone->mWeights[i] = tempBone->mWeights[i];
	}
	return bone;
}

void Assimp_Loader::LoadTexture(const aiScene *tempScene) {

}

void Assimp_Loader::LoadNode(aiNode* parent, aiNode* tempNode) {
	for (int i = 0; i < parent->mNumChildren; i++) {
		aiNode *node = new aiNode;
		node->mParent = parent;
		node->mName = tempNode->mChildren[i]->mName;
		node->mNumChildren = tempNode->mChildren[i]->mNumChildren;
		node->mNumMeshes = tempNode->mChildren[i]->mNumMeshes;
		node->mTransformation = tempNode->mChildren[i]->mTransformation;
		node->mMeshes = new uint[node->mNumMeshes];
		for (int j = 0; j < node->mNumMeshes; j++)
			node->mMeshes[j] = tempNode->mChildren[i]->mMeshes[j];
		node->mChildren = new aiNode*[node->mNumChildren];
		parent->mChildren[i] = node;
		LoadNode(node, tempNode->mChildren[i]);
	}
}