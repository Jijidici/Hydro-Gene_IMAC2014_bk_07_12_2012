#version 330

uniform ivec2 uNbIntersection = ivec2(1.f);
uniform vec3 uNormSum = vec3(1.);

out vec4 fFragColor;

vec3 light = vec3(-1.,0.5,1.);

void main() {
	
	float NdotL = dot(normalize(uNormSum/uNbIntersection[0]), light) / 2.f;
	
	//float ratioIntersection = uNbIntersection[0] / float(uNbIntersection[1]);
	//fFragColor = vec4(1.f, 1.f - ratioIntersection, 0.1f, 1.f);
	//fFragColor = vec4(1.f, 1.f * NdotL, 1.f, 1.f);
	fFragColor = vec4(1.f * NdotL, 1.f * NdotL, 1.f * NdotL, 1.f);
}
