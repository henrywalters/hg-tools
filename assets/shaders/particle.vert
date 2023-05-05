#version 300 es

precision highp float;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture;
layout (location = 3) in vec3 a_pos;
layout (location = 4) in vec3 a_velocity;
layout (location = 5) in vec4 a_color;
layout (location = 6) in float a_startTime;
layout (location = 7) in float a_aliveFor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float currentTime;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 Color;

void main() {
    float elapsedTime = currentTime - a_startTime;
    gl_Position = projection * view * model * vec4((a_pos + a_velocity * elapsedTime) + a_vertex, 1.0);
    Normal = a_normal;
    FragPos = vec3(gl_Position);
    if (elapsedTime >= a_aliveFor) {
        Color = vec4(0, 0, 0, 0);
    } else {
        Color = vec4(a_color.xyz, 1.0 - (elapsedTime / a_aliveFor));
    }
    TexCoord = a_texture;
}