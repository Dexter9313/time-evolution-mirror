#version 330 core

// space-time positions
in vec4 pos_prev;
in float color_prev;
in vec4 pos_next;
in float color_next;

uniform mat4 camera;

uniform float time;

const float boxSize = 4564.667500421165;

out vec4 f_color;

vec3 wrapB(vec3 v, vec3 b)
{
	vec3 result;
	result.x = (v.x < 0.0) ? v.x + b.x : (v.x >= b.x ? v.x - b.x : v.x);
	result.y = (v.y < 0.0) ? v.y + b.y : (v.y >= b.y ? v.y - b.y : v.y);
	result.z = (v.z < 0.0) ? v.z + b.z : (v.z >= b.z ? v.z - b.z : v.z);
	return result;
}

vec3 wrapAB(vec3 v, vec3 a, vec3 b)
{
	vec3 result;
	result.x = (v.x < a.x) ? v.x + (b.x - a.x)
	                       : (v.x >= b.x ? v.x - (b.x - a.x) : v.x);
	result.y = (v.y < a.y) ? v.y + (b.y - a.y)
	                       : (v.y >= b.y ? v.y - (b.y - a.y) : v.y);
	result.z = (v.z < a.z) ? v.z + (b.z - a.z)
	                       : (v.z >= b.z ? v.z - (b.z - a.z) : v.z);
	return result;
}

void main()
{
	float t = (time - pos_prev.w) / (pos_next.w - pos_prev.w);

	vec3 dir = (pos_next.xyz - pos_prev.xyz);

	dir = wrapAB(dir, vec3(-boxSize / 2.0), vec3(boxSize / 2.0));

	vec3 position = wrapB(pos_prev.xyz + t * dir, vec3(boxSize));

	gl_Position = camera * vec4(position, 1.0);

	vec4 colprev = color_prev * vec4(1.0, 0.0, 0.0, 0.1)
	               + (1.0 - color_prev) * vec4(0.0, 1.0, 0.0, 0.1);
	vec4 colnext = color_next * vec4(1.0, 0.0, 0.0, 0.1)
	               + (1.0 - color_next) * vec4(0.0, 1.0, 0.0, 0.1);

	f_color = t * colnext + (1.0 - t) * colprev;
}
