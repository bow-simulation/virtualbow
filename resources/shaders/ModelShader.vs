#version 330 core

layout (location = 0) in vec3 modelPosition;
layout (location = 1) in vec3 modelNormal;
layout (location = 2) in vec3 modelColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    FragPos = vec3(modelMatrix*vec4(modelPosition, 1.0));
    Normal = normalMatrix*modelNormal;
    Color = modelColor;

    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(modelPosition.xyz, 1.0);
}
