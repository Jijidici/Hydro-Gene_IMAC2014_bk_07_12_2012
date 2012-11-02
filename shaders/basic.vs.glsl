#version 330

layout(location = 0) in vec3 position;

uniform mat4 uMVPMatrix = mat4(1.f);
uniform ivec2 uNbIntersection;

out vec3 oPosition;

void main(){
	oPosition = position;
	
	if(uNbIntersection[0] > 0){
		gl_Position = uMVPMatrix * vec4(position, 1.f);
	}
}
