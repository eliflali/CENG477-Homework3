// Vertex Shader
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fragWorldNor;
out vec3 fragWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragWorldPos = vec3(model * vec4(aPos, 1.0)); // Renamed from FragPos to fragWorldPos
    fragWorldNor = mat3(transpose(inverse(model))) * aNormal; // Renamed from Normal to fragWorldNor
    gl_Position = projection * view * vec4(fragWorldPos, 1.0);
}
