#include "Render_Environment.h"

Render_Environment::Render_Environment() {
	squareVertex[0] = aiVector3D(0.0f, 0.0f, 0.0f); squareVertex[1] = aiVector3D(1.0f, 0.0f, 0.0f); squareVertex[2] = aiVector3D(1.0f, 1.0f, 0.0f);
	squareVertex[3] = aiVector3D(0.0f, 0.0f, 0.0f); squareVertex[4] = aiVector3D(1.0f, 1.0f, 0.0f); squareVertex[5] = aiVector3D(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < NUMBER_OF_CUBE_FACES; i++) {
		squareNormal[i] = aiVector3D(0.0f, 0.0f, 1.0f);
	}
	// back
	squareUV[0] = aiVector2D(0.25f, 0.333333f); squareUV[1] = aiVector2D(0.5f, 0.333333f); squareUV[2] = aiVector2D(0.5f, 0.666666f);
	squareUV[3] = aiVector2D(0.25f, 0.333333f); squareUV[4] = aiVector2D(0.5f, 0.666666f); squareUV[5] = aiVector2D(0.25f, 0.666666f);
	// right
	squareUV[6] = aiVector2D(0.5f, 0.333333f); squareUV[7] = aiVector2D(0.75f, 0.333333f); squareUV[8] = aiVector2D(0.75f, 0.666666f);
	squareUV[9] = aiVector2D(0.5f, 0.333333f); squareUV[10] = aiVector2D(0.75f, 0.666666f); squareUV[11] = aiVector2D(0.5f, 0.666666f);
	// front
	squareUV[12] = aiVector2D(0.75f, 0.333333f); squareUV[13] = aiVector2D(1.0f, 0.333333f); squareUV[14] = aiVector2D(1.0f, 0.666666f);
	squareUV[15] = aiVector2D(0.75f, 0.333333f); squareUV[16] = aiVector2D(1.0f, 0.666666f); squareUV[17] = aiVector2D(0.75f, 0.666666f);
	// left
	squareUV[18] = aiVector2D(0.0f, 0.333333f); squareUV[19] = aiVector2D(0.25f, 0.333333f); squareUV[20] = aiVector2D(0.25f, 0.666666f);
	squareUV[21] = aiVector2D(0.0f, 0.333333f); squareUV[22] = aiVector2D(0.25f, 0.666666f); squareUV[23] = aiVector2D(0.0f, 0.666666f);
	// up
	squareUV[24] = aiVector2D(0.25f, 0.666666f); squareUV[25] = aiVector2D(0.5f, 0.666666f); squareUV[26] = aiVector2D(0.5f, 1.0f);
	squareUV[27] = aiVector2D(0.25f, 0.666666f); squareUV[28] = aiVector2D(0.5f, 1.0f); squareUV[29] = aiVector2D(0.25f, 1.0f);
	// down
	squareUV[30] = aiVector2D(0.25f, 0.0f); squareUV[31] = aiVector2D(0.5f, 0.0f); squareUV[32] = aiVector2D(0.5f, 0.333333f);
	squareUV[33] = aiVector2D(0.25f, 0.0f); squareUV[34] = aiVector2D(0.5f, 0.333333f); squareUV[35] = aiVector2D(0.25f, 0.333333f);

	terrainUV[0] = aiVector2D(0.0f, 0.0f); terrainUV[1] = aiVector2D(1.0f, 0.0f); terrainUV[2] = aiVector2D(1.0f, 1.0f);
	terrainUV[3] = aiVector2D(0.0f, 0.0f); terrainUV[4] = aiVector2D(1.0f, 1.0f); terrainUV[5] = aiVector2D(0.0f, 1.0f);
	
	CubeMatrix[0] = translate(mat4(1.0f), vec3(0.0f, 0.0f, -0.5f));
	CubeMatrix[1] = translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.0f));
	CubeMatrix[1] = rotate(CubeMatrix[1], radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
	CubeMatrix[2] = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.5f));
	CubeMatrix[2] = rotate(CubeMatrix[2], radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	CubeMatrix[3] = translate(mat4(1.0f), vec3(-0.5f, 0.0f, 0.0f));
	CubeMatrix[3] = rotate(CubeMatrix[3], radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	CubeMatrix[4] = translate(mat4(1.0f), vec3(0.0f, 0.5f, 0.0f));
	CubeMatrix[4] = rotate(CubeMatrix[4], radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	CubeMatrix[5] = translate(mat4(1.0f), vec3(0.0f, -0.5f, 0.0f));
	CubeMatrix[5] = rotate(CubeMatrix[5], radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

	for (int i = 0; i < NUMBER_OF_CUBE_FACES; i++) {
		CubeMatrix[i] = translate(CubeMatrix[i], vec3(-0.5f, -0.5f, 0.0f));
	}
}

void Render_Environment::Initiate(GLuint program, Variables *globalVar) {
	shaderProgram = program;
	var = globalVar;
}

void Render_Environment::LoadTexture(const char *filename, bool arrayMode = false, GLint z = 0) {

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

	if(arrayMode == false)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	else {
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, z, width, height, 1, GL_BGRA, GL_UNSIGNED_BYTE, &clear_data[0]);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, z, width, height, 1, GL_BGRA, GL_UNSIGNED_BYTE, data);
	}
	
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

void Render_Environment::LoadTextureHint(const char *filename) {

	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP *tx_pixmap, *tx_pixmap_32;

	int width, height;
	GLvoid *data;

	if (*filename == NULL) {
		return;
	}
	tx_file_format = FreeImage_GetFileType(filename, 0);
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);

	heightmapTextureSize.x = width;
	heightmapTextureSize.y = height;
}

void Render_Environment::LoadSkymap(const char *filePath) {
	skyMap = filePath;
	GenRenderObjects();
	BindRenderData();
}

void Render_Environment::DrawSkymap() {
	glUseProgram(shaderProgram);
	glPointSize(5.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	for (int i = 0; i < NUMBER_OF_CUBE_FACES; i++) {
		glBindVertexArray(vao_skybox[i]);
		var->ModelMatrix = glm::scale(mat4(1.0f), vec3(10000.0f, 10000.0f, 10000.0f)) * CubeMatrix[i];
		SetShaderVariables_skybox();
		
		glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_CUBE_FACES * 3);
		glBindVertexArray(0);
	}
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}

void Render_Environment::GenRenderObjects() {
	vao_skybox.resize(NUMBER_OF_CUBE_FACES, 0);
	vbo_skybox.resize(NUMBER_OF_CUBE_FACES * 3, 0);
	textureID_skybox.resize(1);
	glGenVertexArrays(NUMBER_OF_CUBE_FACES, &vao_skybox[0]);
	glGenBuffers(NUMBER_OF_CUBE_FACES * 3, &vbo_skybox[0]);
}

void Render_Environment::BindRenderData(){
	
	for (int i = 0; i < NUMBER_OF_CUBE_FACES; i++) {
		glBindVertexArray(vao_skybox[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox[i * 3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 3, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 3, &squareVertex[0][0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox[i * 3 + 1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 3, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 3, &squareNormal[0][0]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox[i * 3 + 2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 2, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 2, &squareUV[i * NUMBER_OF_SQUARE_VERTICES][0]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		
		glBindVertexArray(0);
	}

	glGenTextures(1, &textureID_skybox[0]);
	int currentActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &currentActiveTexture);
	glActiveTexture(currentActiveTexture + 1);
	glBindTexture(GL_TEXTURE_2D, textureID_skybox[0]);
	LoadTexture(skyMap.c_str());
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Render_Environment::SetShaderVariables_skybox() {
	var->ModelViewMatrix = var->EyeMatrix * var->ModelMatrix;
	var->ModelViewProjectionMatrix = var->ProjectionMatrix * var->ModelViewMatrix;
	
	glUniform1i(var->loc_tex, textureID_skybox[0]);
	glUniformMatrix4fv(var->loc_MVP, 1, GL_FALSE, &var->ModelViewProjectionMatrix[0][0]);
}

void Render_Environment::GenTerrainBuffer() {

	vao_terrain.resize(1);
	vbo_terrain.resize(3);

	glGenVertexArrays(1, &vao_terrain[0]);
	glGenBuffers(3, &vbo_terrain[0]);

	glBindVertexArray(vao_terrain[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo_terrain[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * HEIGHTMAP_HEIGHT * HEIGHTMAP_WIDTH * NUMBER_OF_SQUARE_VERTICES * 3, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * HEIGHTMAP_HEIGHT * HEIGHTMAP_WIDTH * NUMBER_OF_SQUARE_VERTICES * 3, &terrainVertex[0][0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_terrain[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 3, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 3, &squareNormal[0][0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_terrain[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUMBER_OF_SQUARE_VERTICES * 2, &terrainUV[0][0]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Render_Environment::DrawTerrain(Variables *globalVar) {
	var = globalVar;
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shaderProgram);
	var->ModelMatrix = scale(mat4(1.0f), vec3(100.0f, 100.0f, 100.0f));
	var->ModelMatrix *= rotate(mat4(1.0f), radians(-90.0f), var->xAxisVector);
	glBindVertexArray(vao_terrain[0]);
	SetShaderVariables_terrain();

	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_SQUARE_VERTICES * HEIGHTMAP_HEIGHT * HEIGHTMAP_WIDTH * NUMBER_OF_SQUARE_VERTICES * 3);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Render_Environment::SetShaderVariables_terrain() {
	
	var->ModelViewMatrix = var->EyeMatrix * var->ModelMatrix;
	var->ModelViewProjectionMatrix = var->ProjectionMatrix * var->ModelViewMatrix;
	var->ModelViewInverseMatrix = mat3(inverseTranspose(var->ModelViewMatrix));

	glUniform1i(var->loc_heightmapArray, textureID_heightmap[0]); // Heightmap texture
	glUniform1i(var->loc_terrainTexture, textureID_terrain[0]); // Terrain texture
	glUniform1f(var->loc_heightmapSize, HEIGHTMAP_SIZE);

	glUniform1i(var->useTexture, false);
	glUniform4fv(var->loc_ambient, 1, var->testColor);
	glUniform4fv(var->loc_diffuse, 1, var->testColor);
	glUniform4fv(var->loc_specular, 1, var->testColor);
	glUniform3fv(var->loc_sunPosition, 1, var->sunPosition);
	glUniform3fv(var->loc_eyePosition, 1, &var->eyePosition[0]);
	glUniformMatrix4fv(var->loc_M, 1, GL_FALSE, &var->ModelMatrix[0][0]);
	glUniformMatrix4fv(var->loc_MV, 1, GL_FALSE, &var->ModelViewMatrix[0][0]);
	glUniformMatrix4fv(var->loc_MVP, 1, GL_FALSE, &var->ModelViewProjectionMatrix[0][0]);
	glUniformMatrix3fv(var->loc_MVI, 1, GL_FALSE, &var->ModelViewInverseMatrix[0][0]);
	glUniform2fv(var->loc_heightmapTextureSize, 1, &heightmapTextureSize[0]);
}

void Render_Environment::GenTerrainMesh(const char *filePath) {
	clear_data.resize(MAX_TEXTURE_WIDTH * MAX_TEXTURE_HEIGHT, 0);

	heightMap = filePath;

	terrainVertex.resize(HEIGHTMAP_HEIGHT * HEIGHTMAP_WIDTH * NUMBER_OF_SQUARE_VERTICES);

	for (int i = 0; i < HEIGHTMAP_HEIGHT; i++) {
		for (int j = 0; j < HEIGHTMAP_WIDTH; j++) {
			for (int k = 0; k < NUMBER_OF_SQUARE_VERTICES; k++) {
				//terrainVertex[NUMBER_OF_SQUARE_VERTICES * (j + HEIGHTMAP_HEIGHT * i) + k] = squareVertex[k] + aiVector3D(j - (float)HEIGHTMAP_WIDTH / 2.0f, i - (float)HEIGHTMAP_HEIGHT / 2.0f, 0);
				terrainVertex[NUMBER_OF_SQUARE_VERTICES * (j + HEIGHTMAP_HEIGHT * i) + k] = squareVertex[k] + aiVector3D(j, i, 0);
			}
		}
	}

	GenTerrainBuffer();
	GenHeightmap();
	vector<aiVector3D> dump;
	terrainVertex.swap(dump);
	GenTerrainTexture();
}

void Render_Environment::GenHeightmap() {
	textureID_heightmap.resize(1);

	glGenTextures(1, &textureID_heightmap[0]);
	
	int currentActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &currentActiveTexture);
	glActiveTexture(currentActiveTexture + 1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID_heightmap[0]);
	LoadTextureHint(heightMap.c_str());
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 10, GL_RGBA8, 512, 512, 10);
	LoadTexture(heightMap.c_str(), true);
	LoadTexture("Texture/Heightmap/Alphamap.png", true, 1);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void Render_Environment::GenTerrainTexture() {
	textureID_terrain.resize(1);

	glGenTextures(1, &textureID_terrain[0]);

	int currentActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &currentActiveTexture);
	glActiveTexture(currentActiveTexture + 1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID_terrain[0]);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 10, GL_RGBA8, 512, 512, 10);
	LoadTexture("Texture/Terrain/Terrain_tex6.jpg", true, 5);
	LoadTexture("Texture/Terrain/Terrain_tex21.png", true, 8);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -0.5f);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}