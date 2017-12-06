#include "Render_Object.h"

Render_Object::Render_Object():boneNumber(0), animate(false), instancingFlag(false) {}
Render_Object::Render_Object(const char *filePath):boneNumber(0), animate(false), instancingFlag(false) {
	LoadFile(filePath);
}

void Render_Object::LoadTexture(const char *filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP *tx_pixmap, *tx_pixmap_32;

	int width, height;
	GLvoid *data;

	if (*filename == NULL) {
		return;
	}
	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		//fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

// Scene initializer
void Render_Object::InitScene(GLuint program, Variables *globalVar, bool animateFlag = false, bool instancing = false) {
	animate = animateFlag;
	instancingFlag = instancing;
	// Initializer for unknown Assimp loader error handling
	cout << scene->mNumMeshes << " meshes detected!" << endl;
	var = globalVar;
	SetShaderProgram(program);
	GenRenderObjects();
	BindRenderData();
	GenTextures();
	GenBoneData();
	GenGlobalInverseMatrix();

	GenBoneMatrix(scene->mRootNode, var->UnitMatrix);
}

void Render_Object::LoadTransformData(const char *filePath) {
	FILE *file;
	fopen_s(&file, filePath, "rb");
	char buffer[100];
	fgets(buffer, 100, file);
	sscanf_s(buffer, "%d", &numInstance);
	vector<mat4> tMatrix, rMatrix, sMatrix;
	tMatrix.resize(numInstance);
	rMatrix.resize(numInstance);
	sMatrix.resize(numInstance);
	vector<mat4> TotalMatrix;
	TotalMatrix.resize(numInstance);

	for (int i = 0; i < numInstance; i++) {
		GLfloat tTemp[3], rTemp, sTemp[3];
		fgets(buffer, 100, file);
		sscanf_s(buffer, "%f %f %f", &tTemp[0], &tTemp[1], &tTemp[2]);
		
		fgets(buffer, 100, file);
		sscanf_s(buffer, "%f", &rTemp);

		fgets(buffer, 100, file);
		sscanf_s(buffer, "%f %f %f", &sTemp[0], &sTemp[1], &sTemp[2]);
		
		tMatrix[i] = translate(mat4(1.0f), vec3(tTemp[0], tTemp[1], tTemp[2]));
		rMatrix[i] = rotate(mat4(1.0f), radians(rTemp), vec3(0.0f, 0.0f, 1.0f));
		sMatrix[i] = scale(mat4(1.0f), vec3(sTemp[0], sTemp[1], sTemp[2]));
		TotalMatrix[i] = tMatrix[i] * rMatrix[i] * sMatrix[i];
	}

	/*for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << TotalMatrix[0][1][4 * i + j] << " ";
		}
		cout << endl;
	}*/

	for (int i = 0; i < scene->mNumMeshes; i++) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[NUMBER_OF_MESH_VBO * i + 5]);
		for (int j = 0; j < 4; j++) {
			glVertexAttribPointer(5 + j, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (GLvoid*)(sizeof(GLfloat) * j * 4));
			glEnableVertexAttribArray(5 + j);
			glVertexAttribDivisor(5 + j, 1);
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * numInstance, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * numInstance, &TotalMatrix[0][0][0]);
	}
	
	fclose(file);
}

void Render_Object::InstanceDraw(Variables *globalVar, bool animate) {

}

int Render_Object::LoadFile(const char *filePath) {
	scene = importer.LoadScene(filePath, aiProcess_Triangulate | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);
	if (scene == NULL) {
		cout << "Failed to load file" << endl;
		return 0;
	}
	else return 1;
}

void Render_Object::Draw(Variables *globalVar, bool animateFlag = false) {
	animate = animateFlag;
	var = globalVar;
	if (animate == true)
		CalcTimeElapsed();
	glUseProgram(shaderProgram);
	GenBoneMatrix(scene->mRootNode, var->TransMatrix * var->RotMatrix * var->ScaleMatrix);
}

void Render_Object::StartAnimation() {
	timeElapsed = 0.0f;
	timestamp = clock();
	animate == true;
	//currentTick = 0.0f;
	currentTime = 0;
}

void Render_Object::StopAnimation() {
	animate == false;
}

void Render_Object::CalcTimeElapsed() {
	int tempTimestamp = clock();
	timeElapsed = tempTimestamp - timestamp;
	timestamp = tempTimestamp;
	currentTime += timeElapsed;
	//cout << currentTime << endl;
}

void Render_Object::GenRenderObjects() {
	vao.resize(scene->mNumMeshes);
	vbo = new GLuint[scene->mNumMeshes * NUMBER_OF_MESH_VBO];
	vao_index = new GLuint;
	glGenVertexArrays(scene->mNumMeshes, &vao[0]);
	glGenBuffers(scene->mNumMeshes * NUMBER_OF_MESH_VBO, vbo);
	glGenBuffers(1, vao_index);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		boneNumber += scene->mMeshes[i]->mNumBones;
	}

	vao_bone = new GLuint[boneNumber];
	vbo_bone.resize(scene->mNumMeshes * NUMBER_OF_BONE_VBO);
	//vbo_bone = new GLuint[scene->mNumMeshes * NUMBER_OF_BONE_VBO];
	glGenVertexArrays(boneNumber, vao_bone);
	glGenBuffers(boneNumber * NUMBER_OF_BONE_VBO, &vbo_bone[0]);
}

void Render_Object::BindRenderData() {
	
	for (int i = 0; i < scene->mNumMeshes; i++) {

		glBindVertexArray(vao[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[NUMBER_OF_MESH_VBO * i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3, scene->mMeshes[i]->mVertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[NUMBER_OF_MESH_VBO * i + 1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3, scene->mMeshes[i]->mNormals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[NUMBER_OF_MESH_VBO * i + 2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * scene->mMeshes[i]->mNumVertices * 3, &scene->mMeshes[i]->mTextureCoords[0][0][0]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
		
		if(animate == true)
			BindWeightArrays(i);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// TODO : vao index mode
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao_index[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(int) * scene->mMeshes[i]->

		glBindVertexArray(0);
	}
}

void Render_Object::GenTextures() {
	static int numTexture = 0;
	aiString texPath;

	for (int i = 0; i < scene->mNumMaterials; i++) {
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
			numTexture++;
			texPathMap[texPath.data] = 0;
			glGenTextures(1, &texPathMap[texPath.data]);
			int currentActiveTexture;
			glGetIntegerv(GL_ACTIVE_TEXTURE, &currentActiveTexture);
			glActiveTexture(currentActiveTexture + 1);
			glBindTexture(GL_TEXTURE_2D, texPathMap[texPath.data]);
			LoadTexture(texPath.data);
			glGenerateMipmap(GL_TEXTURE_2D);
			//cout << texPath.data<<endl;
			/*cout << currentActiveTexture + 1 - GL_TEXTURE0 << endl;
			cout << texPathMap[texPath.data] << endl;*/
		}
	}
}

void Render_Object::BindWeightArrays(int meshIndex) {

	vector<float> weightArray;
	weightArray.resize(scene->mMeshes[meshIndex]->mNumVertices * MAX_WEIGHT_BONE_NUMBER, 0);
	vector<glm::uint> boneIdArray;
	boneIdArray.resize(scene->mMeshes[meshIndex]->mNumVertices * MAX_WEIGHT_BONE_NUMBER, 0);

	for (int j = 0; j < scene->mMeshes[meshIndex]->mNumBones; j++) {
		aiBone *bones = scene->mMeshes[meshIndex]->mBones[j];
		for (int k = 0; k < bones->mNumWeights; k++) {
			aiVertexWeight weight = bones->mWeights[k];
			for (int l = 0; l < MAX_WEIGHT_BONE_NUMBER; l++) {
				if (weightArray[MAX_WEIGHT_BONE_NUMBER * weight.mVertexId + l] == 0) {
					weightArray[MAX_WEIGHT_BONE_NUMBER * weight.mVertexId + l] = weight.mWeight;
					boneIdArray[MAX_WEIGHT_BONE_NUMBER * weight.mVertexId + l] = j;
					break;
				}
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[NUMBER_OF_MESH_VBO * meshIndex + 3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->mMeshes[meshIndex]->mNumVertices * MAX_WEIGHT_BONE_NUMBER, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * scene->mMeshes[meshIndex]->mNumVertices * MAX_WEIGHT_BONE_NUMBER, &weightArray[0]);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[NUMBER_OF_MESH_VBO * meshIndex + 4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::uint) * scene->mMeshes[meshIndex]->mNumVertices * MAX_WEIGHT_BONE_NUMBER, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::uint) * scene->mMeshes[meshIndex]->mNumVertices * MAX_WEIGHT_BONE_NUMBER, &boneIdArray[0]);
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, 4 * sizeof(uInt), (GLvoid*)0);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
}

// Generate global offset bone matrix from assimp, initialize global currentkey global bone matrix
void Render_Object::GenBoneData() {
	meshData.resize(scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; i++) {
		// set current mesh pointer
		aiMesh *currentMesh = scene->mMeshes[i];

		vector<glm::mat4> boneMatrix;
		boneMatrix.resize(currentMesh->mNumBones, glm::mat4(1.0f));

		for (int j = 0; j < currentMesh->mNumBones; j++) {
			// set current bone pointer
			aiBone *currentBone = currentMesh->mBones[j];

			boneMatrix[j] = transpose(gf.AiToGlm(currentBone->mOffsetMatrix));

			meshData[i].boneMap[currentBone->mName.data] = j;
		}

		// copy bone data
		meshData[i].GlobalBoneMatrix = boneMatrix;
		meshData[i].OffsetBoneMatrix = boneMatrix;
	}
}

// Generate inverse matrix of root node
void Render_Object::GenGlobalInverseMatrix() {
	GlobalInverseMatrix = inverse(gf.AiToGlm(scene->mRootNode->mTransformation));
}

aiAnimation* Render_Object::FindAnimation(aiNode *node) {
	if (scene->mNumAnimations > 0) {
		return scene->mAnimations[0];
	}
	else return nullptr;
}

aiNodeAnim* Render_Object::FindNodeAnim(aiAnimation *animation, const char *nodeName) {
	if (animation == nullptr)
		return nullptr;
	
	for (int i = 0; i < animation->mNumChannels; i++) {
		if (strcmp(animation->mChannels[i]->mNodeName.data, nodeName) == 0) {
			return animation->mChannels[i];
			break;
		}
	}
	return nullptr;
}

void Render_Object::GenBoneMatrix(aiNode *currentNode, mat4 ParentModelTransform) {

	aiAnimation *currentAnim = FindAnimation(currentNode);
	aiNodeAnim *nodeAnim = FindNodeAnim(currentAnim, currentNode->mName.data);

	mat4 CurrentModelTransform;
	if ((nodeAnim != nullptr) && (animate == true)) {
		float currentTick = (float)currentTime / 1000.0f * currentAnim->mTicksPerSecond;
		if (currentTick > currentAnim->mDuration) {
			currentTick -= currentAnim->mDuration;
			currentTime = 0;
		}
		CurrentModelTransform = ParentModelTransform * CalcTMat(currentTick, nodeAnim, currentAnim->mDuration) * CalcRMat(currentTick, nodeAnim, currentAnim->mDuration) * CalcSMat(currentTick, nodeAnim, currentAnim->mDuration);
	}
	else {
		CurrentModelTransform = ParentModelTransform * transpose(gf.AiToGlm(currentNode->mTransformation));
	}

	if (animate == true) {
		for (int i = 0; i < scene->mNumMeshes; i++) {
			glm::uint boneIndex;
			if (meshData[i].boneMap.find(currentNode->mName.data) != meshData[i].boneMap.end()) {
				boneIndex = meshData[i].boneMap[currentNode->mName.data];
				meshData[i].GlobalBoneMatrix[boneIndex] =
					GlobalInverseMatrix * CurrentModelTransform * meshData[i].OffsetBoneMatrix[boneIndex];
				break;
			}
		}
	}

	// * Comment : Despite of the delay of meshes, render while bone calculation for real-time rendering performence
	// * To remove delay, do bone calculation first before drawing mesh

	for (int i = 0; i < currentNode->mNumMeshes; i++) {
		glBindVertexArray(vao[currentNode->mMeshes[i]]);
		SetShaderVariables(scene->mMeshes[currentNode->mMeshes[i]]->mMaterialIndex, CurrentModelTransform, currentNode->mMeshes[i]);

		glPointSize(5.0f);
		if (instancingFlag == true)
			glDrawArraysInstanced(GL_TRIANGLES, 0, scene->mMeshes[currentNode->mMeshes[i]]->mNumVertices * 3, numInstance);
		else glDrawArrays(GL_TRIANGLES, 0, scene->mMeshes[currentNode->mMeshes[i]]->mNumVertices * 3);
		glBindVertexArray(0);
	}

	for (int i = 0; i < currentNode->mNumChildren; i++) {
		GenBoneMatrix(currentNode->mChildren[i], CurrentModelTransform);
	}
}

mat4 Render_Object::CalcRMat(float tick, aiNodeAnim *nodeAnim, float duration) {
	float scaleFactor = (float)nodeAnim->mNumRotationKeys / duration;
	glm::uint leftTick = tick * scaleFactor;
	glm::uint rightTick = (tick + 1) * scaleFactor;
	float interpolateFactor = tick * scaleFactor - (float)leftTick;
	aiQuaternion rQuat;
	if (rightTick > nodeAnim->mNumRotationKeys - 1)
		rightTick = leftTick;
	if (nodeAnim->mNumRotationKeys > 1) {
		aiQuaternion::Interpolate(rQuat, nodeAnim->mRotationKeys[leftTick].mValue, nodeAnim->mRotationKeys[rightTick].mValue, interpolateFactor);
	}
	else {
		rQuat = nodeAnim->mRotationKeys[0].mValue;
	}

	return transpose(gf.AiToGlm(aiMatrix4x4(rQuat.GetMatrix())));
}

mat4 Render_Object::CalcSMat(float tick, aiNodeAnim *nodeAnim, float duration) {
	float scaleFactor = (float)nodeAnim->mNumScalingKeys / duration;
	glm::uint leftTick = tick * scaleFactor;
	glm::uint rightTick = (tick + 1) * scaleFactor;
	float interpolateFactor = tick * scaleFactor - (float)leftTick;
	if (rightTick > nodeAnim->mNumScalingKeys - 1)
		rightTick = leftTick;
	if (nodeAnim->mNumScalingKeys > 1) {
		aiVector3D vector = nodeAnim->mScalingKeys[leftTick].mValue * (1 - interpolateFactor) + nodeAnim->mScalingKeys[rightTick].mValue * interpolateFactor;
		return translate(mat4(1.0f), vec3(vector.x, vector.y, vector.z));
	}
	else return scale(mat4(1.0f), vec3(nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y, nodeAnim->mScalingKeys[0].mValue.z));
}

mat4 Render_Object::CalcTMat(float tick, aiNodeAnim *nodeAnim, float duration) {
	float scaleFactor = (float)nodeAnim->mNumPositionKeys / duration;
	glm::uint leftTick = tick * scaleFactor;
	glm::uint rightTick = (tick + 1) * scaleFactor;
	float interpolateFactor = tick * scaleFactor - (float)leftTick;
	if (rightTick > nodeAnim->mNumPositionKeys - 1)
		rightTick = leftTick;
	if (nodeAnim->mNumPositionKeys > 1) {
		aiVector3D vector = nodeAnim->mPositionKeys[leftTick].mValue * (1 - interpolateFactor) + nodeAnim->mPositionKeys[rightTick].mValue * interpolateFactor;
		return translate(mat4(1.0f), vec3(vector.x, vector.y, vector.z));
	}
	else return translate(mat4(1.0f), vec3(nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z));
}

void Render_Object::SetShaderVariables(int materialIndex, mat4 ModelMatrix, int meshIndex) {
	var->ModelMatrix = ModelMatrix;
	var->ModelViewMatrix = var->EyeMatrix * ModelMatrix;
	var->ModelViewProjectionMatrix = var->ProjectionMatrix * var->ModelViewMatrix;
	var->ViewProjectionMatrix = var->ProjectionMatrix * var->EyeMatrix;
	var->ModelViewInverseMatrix = inverseTranspose(mat3(var->ModelViewMatrix));
	aiString texPath;
	if (scene->mMaterials[materialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
		glActiveTexture(GL_TEXTURE0 + texPathMap[texPath.data]);
		glBindTexture(GL_TEXTURE_2D, texPathMap[texPath.data]);
		glUniform1i(var->loc_tex, texPathMap[texPath.data]);
		glUniform1i(var->useTexture, true);
	}
	glUniform1i(var->loc_instanceMode, instancingFlag);

	aiColor4D ambient, diffuse, specular;
	aiGetMaterialColor(scene->mMaterials[materialIndex], AI_MATKEY_COLOR_DIFFUSE, &diffuse);
	aiGetMaterialColor(scene->mMaterials[materialIndex], AI_MATKEY_COLOR_AMBIENT, &ambient);
	aiGetMaterialColor(scene->mMaterials[materialIndex], AI_MATKEY_COLOR_SPECULAR, &specular);
	
	glUniform4fv(var->loc_ambient, 1, &ambient[0]);
	glUniform4fv(var->loc_diffuse, 1, &diffuse[0]);
	glUniform4fv(var->loc_specular, 1, &specular[0]);
	glUniform3fv(var->loc_sunPosition, 1, var->sunPosition);
	glUniform3fv(var->loc_eyePosition, 1, &var->eyePosition[0]);
	glUniformMatrix4fv(var->loc_M, 1, GL_FALSE, &var->ModelMatrix[0][0]);
	glUniformMatrix4fv(var->loc_MV, 1, GL_FALSE, &var->ModelViewMatrix[0][0]);
	glUniformMatrix4fv(var->loc_MVP, 1, GL_FALSE, &var->ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(var->loc_VP, 1, GL_FALSE, &var->ViewProjectionMatrix[0][0]);
	glUniformMatrix3fv(var->loc_MVI, 1, GL_FALSE, &var->ModelViewInverseMatrix[0][0]);

	// sub bone matrix to opengl only when mesh has at least one bone matrix
	if (scene->mMeshes[meshIndex]->mNumBones > 0) {
		glUniform1i(var->useBone, true);
		glUniformMatrix4fv(var->loc_BM, scene->mMeshes[meshIndex]->mNumBones, GL_FALSE, &meshData[meshIndex].GlobalBoneMatrix[0][0][0]);
	}
	else {
		glUniform1i(var->useBone, false);
	}
}

void Render_Object::SetShaderProgram(GLuint value) {
	shaderProgram = value;
}

void Render_Object::UpdateVariables(Variables *globalVar) {
	var = globalVar;
}