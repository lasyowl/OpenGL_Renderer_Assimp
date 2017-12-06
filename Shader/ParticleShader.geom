#version 430

layout (points) in;
layout (points, max_vertices = 30) out;

layout (location = 0) in float vs_type[];
layout (location = 1) in vec3 vs_position[];
layout (location = 2) in vec3 vs_velocity[];
layout (location = 3) in float vs_timeRemain[];

layout (location = 0) out float gs_type;
layout (location = 1) out vec3 gs_position;
layout (location = 2) out vec3 gs_velocity;
layout (location = 3) out float gs_timeRemain;

void main(void) {
	gs_type = 2.0f;
	gs_position = vec3(2.0f,0.0f,0.0f);
	gs_velocity = vec3(0.0f, 0.0f, 0.0f);
	gs_timeRemain = 0.0f;
	EmitVertex();
	EndPrimitive();
}