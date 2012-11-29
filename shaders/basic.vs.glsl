#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in float nbIntersection;
layout(location = 2) in vec3 voxPosition;

uniform mat4 uMVPMatrix = mat4(1.f);
uniform float uCubeSize = 2.f;
uniform int uNbIntersectionMax;

out vec3 color;

void main(){
	mat4 finalMatrix = uMVPMatrix * mat4(vec4(uCubeSize, 0.f, 0.f, 0.f),
										 vec4(0.f, uCubeSize, 0.f, 0.f),
										 vec4(0.f, 0.f, uCubeSize, 0.f),
										 vec4(voxPosition.x, voxPosition.y, voxPosition.z, 1.f));

	gl_Position = finalMatrix * vec4(position, 1.f);

	//calcul de couleur
	float ratioIntersection = nbIntersection / float(uNbIntersectionMax);
	color = vec3(1.f, 1.f - ratioIntersection, 0.2f);
}