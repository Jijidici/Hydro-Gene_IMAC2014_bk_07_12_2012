#version 330

in vec3 oPosition;

uniform ivec2 uNbIntersection;

out vec4 fFragColor;

void main() {
	float ratioIntersection = uNbIntersection[0] / float(uNbIntersection[1]);
	vec4 color = vec4(1.f, 1.f, 0.1f, 1.f);
	color.g -= ratioIntersection;
	
	fFragColor = color;
}
