#version 330

in vec3 vNormal;
in vec2 vTexCoords;

out vec4 fFragColor;

void main() {
    fFragColor = vec4(vNormal, 1.f);
}

