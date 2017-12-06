#include "Variables.h"

Variables::Variables() {
	UnitMatrix = mat4(1.0f);
	EyeRotMatrix = mat4(1.0f);
	ScaleMatrix = mat4(1.0f);
	TransMatrix = mat4(1.0f);
	RotMatrix = mat4(1.0f);
	sunPosition[0] = 20000.0f;
	sunPosition[1] = 500.0f;
	sunPosition[2] = 0.0f;
}

void Variables::SetScaleFactor(float S) {
	ScaleMatrix = scale(mat4(1.0f), vec3(S, S, S));
}

void Variables::SetTransFactor(vec3 T) {
	TransMatrix = translate(mat4(1.0f), T);
}