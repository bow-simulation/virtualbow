#version 120

varying vec3 FragPos;
varying vec3 Normal;
varying vec3 Color;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float materialShininess;

void main()
{
    // Diffuse lighting
    vec3 lightDir = normalize(lightPosition - FragPos);
    vec3 normal = normalize(Normal);
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular lighting
    vec3 viewDir = normalize(cameraPosition - FragPos);
    vec3 reflectDir = -reflect(lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);

    vec3 result = (ambientStrength + diffuseStrength*diffuse + specularStrength*specular)*lightColor*Color;
    gl_FragColor = vec4(result, 1.0);
}
