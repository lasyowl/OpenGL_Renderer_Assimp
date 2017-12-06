#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <vector>
#include "Render_Object.h"
#include "Render_Environment.h"
#include "Parse_Shader.h"
#include "Variables.h"
#include "Operator_Overload.h"
#include "GlobalFunctions.h"
#include "Particle_System.h"

using namespace Assimp;
using namespace std;

#define SKYMAP_RELATIVE_SPEED 0.01f
#define ANIMATE_MODE true
#define STATIC_MODE false
#define NORMAL_MODE false
#define INSTANCE_MODE true

enum KeyboardButton { KEYBOARD_a, KEYBOARD_b, KEYBOARD_c, KEYBOARD_d, KEYBOARD_e, KEYBOARD_f, KEYBOARD_g, 
					KEYBOARD_h, KEYBOARD_i, KEYBOARD_j, KEYBOARD_k, KEYBOARD_l, KEYBOARD_m, KEYBOARD_n, 
					KEYBOARD_o, KEYBOARD_p, KEYBOARD_q, KEYBOARD_r, KEYBOARD_s, KEYBOARD_t, KEYBOARD_u, 
					KEYBOARD_v, KEYBOARD_w, KEYBOARD_x, KEYBOARD_y, KEYBOARD_z, KEYBOARD_space = 32,
					MOUSE_LEFT, MOUSE_RIGHT };
bool buttonPressed[100];

Render_Object object, tree, house;
Render_Environment skyMap;
Render_Environment heightMap;
Particle_System particle;
GLuint baseProgram, skymapProgram, heightmapProgram, particleProgram;
GlobalFunctions gf;
Variables var_baseProgram, var_skymapProgram, var_heightmapProgram, var_particleProgram;

ivec2 mouseMotion;
glm::uint eyeMoveSpeed = 10;
float eyeRotSpeed = 0.5f;
vec2 totalDegree(0.0f);

// Get shader program variable locations
void GetShaderVarLocation() {
	// Base shader
	var_baseProgram.loc_M = glGetUniformLocation(baseProgram, "ModelMatrix");
	var_baseProgram.loc_MV = glGetUniformLocation(baseProgram, "ModelViewMatrix");
	var_baseProgram.loc_MVP = glGetUniformLocation(baseProgram, "ModelViewProjectionMatrix");
	var_baseProgram.loc_VP = glGetUniformLocation(baseProgram, "ViewProjectionMatrix");
	var_baseProgram.loc_MVI = glGetUniformLocation(baseProgram, "ModelViewInverseMatrix");
	var_baseProgram.loc_BM = glGetUniformLocation(baseProgram, "BoneMatrix");

	var_baseProgram.loc_tex = glGetUniformLocation(baseProgram, "textureSampler");
	var_baseProgram.useTexture = glGetUniformLocation(baseProgram, "useTexture");
	var_baseProgram.useBone = glGetUniformLocation(baseProgram, "useBone");
	var_baseProgram.loc_instanceMode = glGetUniformLocation(baseProgram, "instanceMode");
	var_baseProgram.loc_ambient = glGetUniformLocation(baseProgram, "color_ambient");
	var_baseProgram.loc_diffuse = glGetUniformLocation(baseProgram, "color_diffuse");
	var_baseProgram.loc_specular = glGetUniformLocation(baseProgram, "color_specular");
	var_baseProgram.loc_sunPosition = glGetUniformLocation(baseProgram, "sunPosition");
	var_baseProgram.loc_eyePosition = glGetUniformLocation(baseProgram, "eyePosition");

	// Skymap shader
	var_skymapProgram.loc_MVP = glGetUniformLocation(skymapProgram, "ModelViewProjectionMatrix");
	var_skymapProgram.loc_tex = glGetUniformLocation(skymapProgram, "textureSampler");

	// Heightmap shader
	var_heightmapProgram.loc_M = glGetUniformLocation(heightmapProgram, "ModelMatrix");
	var_heightmapProgram.loc_MV = glGetUniformLocation(heightmapProgram, "ModelViewMatrix"); 
	var_heightmapProgram.loc_MVP = glGetUniformLocation(heightmapProgram, "ModelViewProjectionMatrix");
	var_heightmapProgram.loc_VP = glGetUniformLocation(heightmapProgram, "ViewProjectionMatrix");
	var_heightmapProgram.loc_MVI = glGetUniformLocation(heightmapProgram, "ModelViewInverseMatrix");
	var_heightmapProgram.loc_heightmapSize = glGetUniformLocation(heightmapProgram, "heightmapSize");
	var_heightmapProgram.loc_heightmapArray = glGetUniformLocation(heightmapProgram, "heightmapArray");
	var_heightmapProgram.loc_heightmapTextureSize = glGetUniformLocation(heightmapProgram, "heightmapTextureSize");
	var_heightmapProgram.loc_terrainTexture = glGetUniformLocation(heightmapProgram, "terrainTexture");
	var_heightmapProgram.loc_sunPosition = glGetUniformLocation(heightmapProgram, "sunPosition");
	var_heightmapProgram.loc_eyePosition = glGetUniformLocation(heightmapProgram, "eyePosition");

	// Particle shader
	var_particleProgram.loc_VP = glGetUniformLocation(particleProgram, "ViewProjectionMatrix");
	var_particleProgram.loc_eyePosition = glGetUniformLocation(particleProgram, "eyePosition");
}

void PrepareShaders() {
	ShaderInfo skymapShader[3] = {
		{ GL_VERTEX_SHADER, "Shader/SkymapShader.vert" },
		{ GL_FRAGMENT_SHADER, "Shader/SkymapShader.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo firstShader[3] = {
		{ GL_VERTEX_SHADER, "Shader/FirstShader.vert" },
		{ GL_FRAGMENT_SHADER, "Shader/FirstShader.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo heightmapShader[4] = {
		{ GL_VERTEX_SHADER, "Shader/Heightmap.vert" },
		{ GL_GEOMETRY_SHADER, "Shader/Heightmap.geom" },
		{ GL_FRAGMENT_SHADER, "Shader/Heightmap.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo particleShader[3] = {
		{ GL_VERTEX_SHADER, "Shader/ParticleShader.vert" },
		{ GL_GEOMETRY_SHADER, "Shader/ParticleShader.geom" },
		{ GL_NONE, NULL }
	};
	Parse_Shader parseShader;
	baseProgram = parseShader.LoadShader(firstShader, false);
	skymapProgram = parseShader.LoadShader(skymapShader, false);
	heightmapProgram = parseShader.LoadShader(heightmapShader, false);
	particleProgram = parseShader.LoadShader(particleShader, true);
	glUseProgram(baseProgram);
	GetShaderVarLocation();
}

void MouseMotion(int x, int y) {
	mouseMotion.x = x;
	mouseMotion.y = y;
}

void MousePassiveMotion(int x, int y) {
	mouseMotion.x = x;
	mouseMotion.y = y;
}

void Mouse(int button, int state, int x, int y) {
	switch (button) {
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN) {
				buttonPressed[MOUSE_RIGHT] = true;
				var_baseProgram.mousePosition.x = x;
				var_baseProgram.mousePosition.y = y;
			}
			else {
				buttonPressed[MOUSE_RIGHT] = false;
			}
			break;
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
				buttonPressed[MOUSE_LEFT] = true;
			else
				buttonPressed[MOUSE_LEFT] = false;
		default:
			break;
	}
}

void MouseWheel(int button, int direction, int x, int y) {
	switch (direction) {
	case -1:
		var_baseProgram.eyePosition -= 50 * var_baseProgram.eyeDirection;
		break;
	case 1:
		var_baseProgram.eyePosition += 50 * var_baseProgram.eyeDirection;
		break;
	default:
		break;
	}
}

void KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		buttonPressed[KEYBOARD_w] = false;
		break;
	case 's':
		buttonPressed[KEYBOARD_s] = false;
		break;
	case 'a':
		buttonPressed[KEYBOARD_a] = false;
		break;
	case 'd':
		buttonPressed[KEYBOARD_d] = false;
		break;
	case 'x':
		buttonPressed[KEYBOARD_x] = false;
		break;
	case 32: // space bar in ascii
		buttonPressed[KEYBOARD_space] = false;
		break;
	case 27: // esc in ascii
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'w':
			buttonPressed[KEYBOARD_w] = true;
			break;
		case 's':
			buttonPressed[KEYBOARD_s] = true;
			break;
		case 'a':
			buttonPressed[KEYBOARD_a] = true;
			break;
		case 'd':
			buttonPressed[KEYBOARD_d] = true;
			break;
		case 'x':
			buttonPressed[KEYBOARD_x] = true;
			break;
		case 32: // space bar in ascii
			buttonPressed[KEYBOARD_space] = true;
			break;
		case 27: // esc in ascii
			glutLeaveMainLoop();
			break;
		default:
			break;
	}
}

void Update() {
	if (gf.FpsCounter() == true) {
		glutPostRedisplay();

		if (buttonPressed[KEYBOARD_w] == true)
			var_baseProgram.eyePosition += eyeMoveSpeed * var_baseProgram.eyeDirection;
		else if (buttonPressed[MOUSE_RIGHT] == true && buttonPressed[MOUSE_LEFT] == true)
			var_baseProgram.eyePosition += eyeMoveSpeed * var_baseProgram.eyeDirection;
		if (buttonPressed[KEYBOARD_s] == true)
			var_baseProgram.eyePosition -= eyeMoveSpeed * var_baseProgram.eyeDirection;
		if (buttonPressed[KEYBOARD_a] == true)
			var_baseProgram.eyePosition += eyeMoveSpeed * cross(var_baseProgram.upVector, var_baseProgram.eyeDirection);
		if (buttonPressed[KEYBOARD_d] == true)
			var_baseProgram.eyePosition -= eyeMoveSpeed * cross(var_baseProgram.upVector, var_baseProgram.eyeDirection);
		if (buttonPressed[KEYBOARD_x] == true)
			var_baseProgram.eyePosition.y -= eyeMoveSpeed;
		if (buttonPressed[KEYBOARD_space] == true)
			var_baseProgram.eyePosition.y += eyeMoveSpeed;

		if (buttonPressed[MOUSE_RIGHT] == true) {

			vec2 rotDegree((float)(var_baseProgram.mousePosition.x - mouseMotion.x) * eyeRotSpeed,
				(float)(var_baseProgram.mousePosition.y - mouseMotion.y) * eyeRotSpeed);

			var_baseProgram.EyeRotMatrix = rotate(radians(rotDegree.x), var_baseProgram.yAxisVector);
			vec3 tempDirection = vec3(var_baseProgram.EyeRotMatrix * vec4(var_baseProgram.eyeDirection, 1.0f));

			if (totalDegree.y + rotDegree.y > 89.9f) {
				rotDegree.y = 89.9f - totalDegree.y;
			}
			if (totalDegree.y + rotDegree.y < -89.9f) {
				rotDegree.y = -89.9f - totalDegree.y;
			}
			totalDegree.y += rotDegree.y;

			vec3 newDirection = cross(var_baseProgram.eyeDirection, vec3(0.0f, 1.0f, 0.0f));
			var_baseProgram.EyeRotMatrix = rotate(radians(rotDegree.y), newDirection);
			var_baseProgram.eyeDirection = vec3(var_baseProgram.EyeRotMatrix * vec4(tempDirection, 1.0f));


			var_baseProgram.mousePosition.x = mouseMotion.x;
			var_baseProgram.mousePosition.y = mouseMotion.y;
		}
	}
}

void Display() {

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, var_baseProgram.backgroundColor);

	var_skymapProgram.EyeMatrix = lookAt(var_baseProgram.eyePosition * SKYMAP_RELATIVE_SPEED, var_baseProgram.eyeDirection + var_baseProgram.eyePosition * SKYMAP_RELATIVE_SPEED, var_baseProgram.yAxisVector);
	var_skymapProgram.ProjectionMatrix = var_baseProgram.ProjectionMatrix;
	//skyMap.DrawSkymap();

	var_heightmapProgram.eyePosition = var_baseProgram.eyePosition;
	var_heightmapProgram.EyeMatrix = lookAt(var_baseProgram.eyePosition, var_baseProgram.eyeDirection + var_baseProgram.eyePosition, var_baseProgram.yAxisVector);
	var_heightmapProgram.ProjectionMatrix = perspective(radians(90.0f), 800.0f / 800.0f, 0.1f, 30000.0f);
	//heightMap.DrawTerrain(&var_heightmapProgram);

	var_baseProgram.EyeMatrix = lookAt(var_baseProgram.eyePosition, var_baseProgram.eyeDirection + var_baseProgram.eyePosition, var_baseProgram.yAxisVector);
	var_baseProgram.ProjectionMatrix = perspective(radians(90.0f), 800.0f / 800.0f, 0.1f, 30000.0f);
	var_baseProgram.SetTransFactor(vec3(0.0f, 0.0f, 0.0f));
	var_baseProgram.SetScaleFactor(1.0f);
	//object.Draw(&var_baseProgram, true);

	//house.Draw(&var_baseProgram, false);
	
	var_baseProgram.SetTransFactor(vec3(500.0f, 0.0f, 250.0f));
	var_baseProgram.SetScaleFactor(0.05f);
	//tree.Draw(&var_baseProgram, false);

	var_particleProgram.ViewProjectionMatrix = var_baseProgram.ViewProjectionMatrix;
	particle.Render(0, &var_particleProgram);

	glutSwapBuffers();
}

void RegisterCallback() {
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutDisplayFunc(Display);
	glutMouseWheelFunc(MouseWheel);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);
	glutIdleFunc(Update);
}

void PrepareRenderObjects() {
	/*object.LoadFile("D:/3D Models/Export/Etc/StickMan.fbx");
	object.InitScene(baseProgram, &var_baseProgram, ANIMATE_MODE, NORMAL_MODE);
	house.LoadFile("D:/3D Models/Export/Etc/seaside-villa.fbx");
	house.InitScene(baseProgram, &var_baseProgram, STATIC_MODE, NORMAL_MODE);
	tree.LoadFile("D:/3D Models/Export/Etc/trees lo-poly.3ds");
	tree.InitScene(baseProgram, &var_baseProgram, STATIC_MODE, INSTANCE_MODE);
	tree.LoadTransformData("Res/test.txt");
	skyMap.LoadSkymap("Texture/Skymap/Skymap_Universe.png");
	skyMap.Initiate(skymapProgram, &var_skymapProgram);
	heightMap.Initiate(heightmapProgram, &var_heightmapProgram);
	heightMap.GenTerrainMesh("Texture/Heightmap/seoul.png");*/
	particle.Initiate(vec3(10.0f, 2.0f, 0.0f), particleProgram, baseProgram);
	printf("%d %d\n", particleProgram, baseProgram);
}

void InitVariables() {
	var_baseProgram.EyeMatrix = lookAt(var_baseProgram.eyePosition, var_baseProgram.eyeDirection + var_baseProgram.eyePosition, var_baseProgram.yAxisVector);
	var_baseProgram.ProjectionMatrix = perspective(radians(90.0f), 800.0f / 800.0f, 0.1f, 10000.0f);
	var_baseProgram.eyePosition = vec3(0.0f, 2.0f, 400.0f);
	var_baseProgram.eyeDirection = vec3(0.0f, 0.0f, -1.0f);
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(2000, 10);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Parse Test");
	glewExperimental = GL_TRUE;

	glewInit();

	InitVariables();
	PrepareShaders();
	PrepareRenderObjects();

	RegisterCallback();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);


	glutMainLoop();

	return 0;
}