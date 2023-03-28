#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:
        Camera();
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix(float width, float height);
        glm::mat4 getViewPortMatrix(float width, float height);

    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        float movementSpeed;
        float mouseSensitivity;
        float fov;
};

#endif /* CAMERA_H */
