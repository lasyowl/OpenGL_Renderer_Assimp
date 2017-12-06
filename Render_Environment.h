#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>

#include <freeImage/FreeImage.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Variables.h"
#include "GlobalFunctions.h"
#include "Assimp_Loader.h"

#define NUMBER_OF_CUBE_FACES 6
#define NUMBER_OF_SQUARE_VERTICES 6

#define HEIGHTMAP_SIZE 128.0f
#define HEIGHTMAP_WIDTH 128
#define HEIGHTMAP_HEIGHT 128
#define MAX_TEXTURE_WIDTH 2048
#define MAX_TEXTURE_HEIGHT 2048

class Render_Environment {
public:
	Render_Environment();

	void Initiate(GLuint program, Variables *var);
	void LoadSkymap(const char *filename);
	void DrawSkymap();
	void GenTerrainMesh(const char *filePath);
	void DrawTerrain(Variables *globalVar);
		
private:
	void LoadTexture(const char *filename, bool arrayMode, GLint z);
	void LoadTextureHint(const char *filename);
	void GenRenderObjects();
	void BindRenderData();
	void SetShaderVariables_skybox();
	void SetShaderVariables_terrain();
	void GenTerrainBuffer();
	void GenHeightmap();
	void GenTerrainTexture();
	
	GlobalFunctions gf;

	aiVector3D squareVertex[6];
	aiVector3D squareNormal[6];
	aiVector2D squareUV[36];
	aiVector2D terrainUV[6];
	glm::mat4 CubeMatrix[6];
	
	std::vector<GLuint> vao_skybox;
	std::vector<GLuint> vbo_skybox;
	std::vector<GLuint> textureID_skybox;

	std::vector<GLuint> vao_terrain;
	std::vector<GLuint> vbo_terrain;
	std::vector<GLuint> textureID_heightmap;
	std::vector<GLuint> textureID_terrain;

	std::vector<aiVector3D> terrainVertex;
	aiVector2D heightmapTextureSize;

	std::string skyMap, heightMap;
	Variables *var;
	GLuint shaderProgram;

	vector<glm::uint> clear_data;
};