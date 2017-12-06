#ifndef __PARSE_SHADER_H
#define __PARSE_SHADER_H

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

#include <GL/glew.h>


struct ShaderInfo {
	GLenum type;
	const char *filename;
	GLuint shader;
};

class Parse_Shader {
public:
	Parse_Shader() {}

	GLuint LoadShader(ShaderInfo *shaderInfo, bool particle);
};

#endif