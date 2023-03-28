#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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


