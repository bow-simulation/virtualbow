#version 120

attribute vec3 modelPosition;
attribute vec3 modelNormal;
attribute vec3 modelColor;

varying vec3 fragColor;

void main()
{
   gl_Position = vec4(modelPosition.xyz, 1.0);
   fragColor = modelColor;
}
