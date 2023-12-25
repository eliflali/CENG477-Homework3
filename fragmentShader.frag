#version 330 core

// Uniforms
uniform vec3 eyePos;
uniform vec3 checkpointColor; // Color for checkpoints
uniform float offset;         // Offset for checkerboard pattern
uniform float scale;          // Scale for checkerboard pattern

// Constants
vec3 I = vec3(1, 1, 1);          // point light intensity
vec3 Iamb = vec3(0.8, 0.8, 0.8); // ambient light intensity
vec3 kd = vec3(1, 1, 0);         // diffuse reflectance coefficient for bunny
vec3 ka = vec3(0.3, 0.3, 0.3);   // ambient reflectance coefficient
vec3 ks = vec3(1, 1, 1);         // specular reflectance coefficient for bunny
vec3 lightPos = vec3(5, 5, 5);   // light position in world coordinates

in vec3 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;

void main(void)
{
    // Compute lighting
    vec3 L = normalize(lightPos - vec3(fragWorldPos));
    vec3 V = normalize(eyePos - vec3(fragWorldPos));
    vec3 H = normalize(L + V);
    vec3 N = normalize(fragWorldNor);

    float NdotL = dot(N, L); // for diffuse component
    float NdotH = dot(N, H); // for specular component

    // Bunny Shader (Diffuse and Specular)
    vec3 diffuseColor = I * kd * max(0, NdotL);
    vec3 specularColor = I * ks * pow(max(0, NdotH), 100);

    // Checkpoint Shader (Diffuse)
    vec3 checkpointDiffuseColor = checkpointColor * max(0, NdotL);

    // Calculate checkerboard pattern for the ground
    bool x = bool(int((fragWorldPos.x + offset) * scale)) != bool(int((fragWorldPos.y + offset) * scale));
    bool y = bool(int((fragWorldPos.y + offset) * scale)) != bool(int((fragWorldPos.z + offset) * scale));
    bool z = bool(int((fragWorldPos.z + offset) * scale));

    vec3 groundColor;
    if (x && y && z) {
        groundColor = vec3(0, 0, 0);
    } else {
        groundColor = vec3(1, 1, 1);
    }

    // Combine colors based on the type of object
    // For the ground, use the checkerboard pattern
    fragColor = vec4(
        (diffuseColor + specularColor + Iamb * ka) * (1.0 - step(0.1, fragWorldPos.y)) + // Bunny
        checkpointDiffuseColor +                                                    // Checkpoint
        groundColor,                                                                 // Ground
        1.0
    );
}
