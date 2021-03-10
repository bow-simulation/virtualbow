#version 120

attribute vec3 modelPosition;
attribute vec3 modelNormal;
attribute vec3 modelColor;

varying vec3 FragPos;
varying vec3 Normal;
varying vec3 Color;

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
