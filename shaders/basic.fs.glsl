#version 330

in vec3 oPosition;

out vec4 fFragColor;

float max(float a, float b){
	if(a>b){ return a; }
	else{ return b; }
}

void main() {
	fFragColor = vec4(oPosition.y*10, 0.1f, 0.2f, 1.f);
}
