#version 330

//flat in int fsNbIntersection;
in vec3 color;

out vec4 fFragColor;

void main() {

	fFragColor = vec4(color, 1.f);
}
