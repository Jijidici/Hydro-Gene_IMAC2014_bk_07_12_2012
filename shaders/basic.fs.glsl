#version 330

in vec3 oPosition;

out vec4 fFragColor;

float max(float a, float b){
	if(a>b){ return a; }
	else{ return b; }
}

void main() {
	fFragColor = vec4(0.6f, 0.5f, 0.f, 1.f);
}
