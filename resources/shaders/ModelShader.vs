#version 330 core

layout (location = 0) in vec3 modelPosition;
layout (location = 1) in vec3 modelNormal;
layout (location = 2) in vec3 modelColor;

out vec3 vertexPosition;
out vec3 vertexNormal;
out vec3 vertexColor;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

void main()
{
    vertexPosition = modelPosition;
    vertexNormal = normalMatrix*modelNormal;
    vertexColor = modelColor;
    gl_Position = projectionMatrix*modelViewMatrix*vec4(modelPosition.xyz, 1.0);
}
