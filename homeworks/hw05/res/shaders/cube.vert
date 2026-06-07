#version 330 core
in vec3 aPos;
in vec2 aUV;

out vec2 vTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vTexCoords = aUV;
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
