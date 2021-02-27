#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 fragColor;

void main()
{
   gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
   fragColor = aColor;
}