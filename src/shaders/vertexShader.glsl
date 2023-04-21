#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModelViewProjectionMatrix;

void main(){
    gl_Position = uModelViewProjectionMatrix * vec4(aPos, 1.0);
}
