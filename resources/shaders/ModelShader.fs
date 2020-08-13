#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float materialShininess;

void main()
{
    /*
    vec3 normalDirection = normalize(vertexNormal);
    vec3 lightDirection = normalize(lightPosition - vertexPosition);
    vec3 viewDirection = normalize(cameraPosition - vertexPosition);
    vec3 reflectDirection = reflect(-lightDirection, normalDirection);

    float diffuse = max(dot(lightDirection, normalDirection), 0.0);
    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), materialShininess);
    vec3 result = (ambientStrength + diffuseStrength*diffuse + specularStrength*specular)*vertexColor;

    gl_FragColor = vec4(result, 1.0);
    */

    // Ambient
    vec3 ambient = ambientStrength*lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse)*Color;
    gl_FragColor = vec4(result, 1.0);
}
