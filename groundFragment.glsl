#version 330 core

in vec4 fragWorldPos; // Position from vertex shader, renamed to match your vertex shader output

uniform float offset; // Offset for the checkerboard
uniform float scale; // Scale for the checkerboard

out vec4 fragColor;

void main() {
    // Compute checkerboard pattern using x and z coordinates
    bool x = int((fragWorldPos.x + offset) * scale) % 2 != 0;
    bool z = int((fragWorldPos.z + offset) * scale) % 2 != 0;
    bool checker = x != z;

    vec3 black = vec3(0.0, 0.0, 0.0); // Black color
    vec3 white = vec3(1.0, 1.0, 1.0); // White color
    fragColor = vec4(checker ? black : white, 1.0);
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
