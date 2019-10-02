#version 330 core

// space-time positions
in vec4 pos_prev;
in vec4 pos_next;

uniform mat4 camera;

uniform float time;

void main()
{
	float t = (time - pos_prev.w) / (pos_next.w - pos_prev.w);
	t = clamp(t, 0.0, 1.0);

	vec3 position = pos_prev.xyz * (1.0-t) + pos_next.xyz * t;
	gl_Position = camera * vec4(position, 1.0);
}
