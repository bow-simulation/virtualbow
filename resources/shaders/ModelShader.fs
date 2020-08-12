#version 330 core

in vec3 ourColor;

void main()
{
    gl_FragColor = vec4(ourColor, 1.0f);
}
