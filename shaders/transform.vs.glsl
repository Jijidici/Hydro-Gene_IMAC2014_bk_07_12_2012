#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture;

uniform mat4 uMVPMatrix = mat4(1.f);

out vec3 vNormal;
out vec2 vTexCoords;

void main(){
	vNormal = normal;
	vTexCoords = texture;
	
	gl_Position = uMVPMatrix * vec4(position, 1.f);
}
