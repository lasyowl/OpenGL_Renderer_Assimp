#version 430

layout (location = 0) in float type;
layout (location = 1) in vec3 position;
layout (location = 2) in vec3 velocity;
layout (location = 3) in float timeRemain;

layout (location = 0) out float vs_type;
layout (location = 1) out vec3 vs_position;
layout (location = 2) out vec3 vs_velocity;
layout (location = 3) out float vs_timeRemain;

void main() {

	vs_type = type;
	vs_position = position;
	vs_velocity  = velocity;
	vs_timeRemain = timeRemain;

}