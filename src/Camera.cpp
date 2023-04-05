#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

#include "Camera.h"


float mouseSensitivity = 0.1f;
float fov = 45.0f;

glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    return glm::lookAt(position, target, up);
}

glm::mat4 getProjectionMatrix(float width, float height) {
    return glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
}



glm::mat4 getViewPortMatrix(float width, float height, glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    glm::mat4 view = getViewMatrix(position, target, up);
    glm::mat4 projection = getProjectionMatrix(width, height);
    glm::mat4 viewPort = glm::mat4(1.0f);

    return viewPort * projection * view;
}

glm::vec3 getTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up, int mouseX, int mouseY, bool lockCamera) {
    if (lockCamera) {
        return target;
    }

    glm::vec3 front = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(front, up));

    float mouseYaw = -(float)mouseX * mouseSensitivity;
    float mousePitch = -(float)mouseY * mouseSensitivity;

    if (mousePitch > 89.0f) {
        mousePitch = 89.0f;
    }

    if (mousePitch < -89.0f) {
        mousePitch = -89.0f;
    }

    float angleX = glm::radians(mousePitch);
    float angleY = glm::radians(mouseYaw);

    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), angleX, right);
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), angleY, up);
    glm::mat4 rotation = rotationX * rotationY;

    glm::vec4 newFront = rotation * glm::vec4(front, 1.0f);

    target = position + glm::vec3(newFront);

    return target;
}


