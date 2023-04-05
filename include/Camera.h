#ifndef CAMERA_H
#define CAMERA_H

extern "C" {

    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    glm::mat4 getProjectionMatrix(float width, float height);
    glm::mat4 getViewPortMatrix(float width, float height, glm::vec3 position, glm::vec3 target, glm::vec3 up);
    glm::vec3 getTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up, int mouseX, int mouseY, bool lockCamera);
 }

#endif /* CAMERA_H */
