#version 330 core

// Define lighting properties
vec3 I = vec3(1, 1, 1);          // Point light intensity
vec3 Iamb = vec3(0.8, 0.8, 0.8); // Ambient light intensity
vec3 lightPos = vec3(5, 5, 5);   // Light position in world coordinates

// Material properties for a golden color
vec3 kd = vec3(1.0, 0.843, 0.0); // Diffuse reflectance (golden)
vec3 ka = vec3(0.3, 0.25, 0.1);  // Ambient reflectance (slightly golden)
vec3 ks = vec3(0.9, 0.9, 0.6);   // Specular reflectance (to add shine)
float shininess = 50.0;          // Shininess coefficient for specular highlights

// Uniform variables from the vertex shader
uniform vec3 eyePos;             // Camera position in world coordinates

// Inputs from the vertex shader
in vec4 fragWorldPos;            // World position of the fragment
in vec3 fragWorldNor;            // World normal of the fragment

// Output color of the fragment
out vec4 fragColor;

void main(void) {
    // Compute vectors for lighting calculations
    vec3 L = normalize(lightPos - vec3(fragWorldPos)); // Light vector
    vec3 V = normalize(eyePos - vec3(fragWorldPos));   // View vector
    vec3 H = normalize(L + V);                         // Halfway vector
    vec3 N = normalize(fragWorldNor);                  // Normal vector

    // Compute lighting components
    float NdotL = max(dot(N, L), 0.0);                 // Diffuse component
    float NdotH = max(dot(N, H), 0.0);                 // Specular component

    // Compute lighting contributions
    vec3 diffuse = kd * I * NdotL;                     // Diffuse reflection
    vec3 specular = ks * I * pow(NdotH, shininess);    // Specular reflection
    vec3 ambient = ka * Iamb;                          // Ambient reflection

    // Combine contributions for final color
    fragColor = vec4(diffuse + specular + ambient, 1.0);
}
