#version 330 core

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec4 vertColor;

uniform mat4 camProjectionMatrix;
uniform mat4 camViewMatrix;

out vec4 oVertColor;

void main()
{
    gl_Position = camProjectionMatrix * camViewMatrix * vec4(vertPosition, 1.0);
    oVertColor = vertColor;
}