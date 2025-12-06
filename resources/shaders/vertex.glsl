#version 330 core

#define RENDERER_BATCH_MAX_OBJECT_COUNT 256 // 16 KB = 16,384 Bytes = 256 * 64

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertUv;

uniform mat4 camProjectionMatrix;
uniform mat4 camViewMatrix;

layout(std140) uniform modelMatrices
{
    mat4 models[RENDERER_BATCH_MAX_OBJECT_COUNT];
};

out vec2 oVertUv;
out vec3 oVertPosition;
out vec3 oVertNormal;

void main()
{
    gl_Position = camProjectionMatrix * camViewMatrix * models[gl_InstanceID] * vec4(vertPosition, 1.0);
    oVertPosition = vec3(models[gl_InstanceID] * vec4(vertPosition, 1.0));
    oVertNormal = mat3(transpose(inverse(models[gl_InstanceID]))) * vertNormal;
    oVertUv = vertUv;
}