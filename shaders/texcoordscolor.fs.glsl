#version 330

in vec3 vNormal;
in vec2 vTexCoords;

out vec4 fFragColor;

void main() {
    fFragColor = vec4(vTexCoords, 0.f, 1.f);
    //fFragColor = vec4(1.f , 1.f, 0.f, 1.f);
}
