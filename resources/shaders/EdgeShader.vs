#version 330 core

layout (location = 0) in vec3 modelPosition;
layout (location = 1) in vec3 modelNormal;
layout (location = 2) in vec3 modelColor;

out vec3 fragColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    fragColor = modelColor;
    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(modelPosition.xyz, 1.0);
}
