#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mouse.h>
#include <stdbool.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "voxelRenderer.h"
#include "Camera.h"

#define false 0
#define true 1

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080



int main(int argc, char* argv[]){
    SDL_Window* window = NULL;
    SDL_GLContext glContext = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    }

    window = SDL_CreateWindow("Voxel Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    glContext = SDL_GL_CreateContext(window);

    GLenum glewError = glewInit();

    if(glewError != GLEW_OK){
        printf("Error initializing GLEW! %s", glewGetErrorString(glewError));
    }

    SDL_Event event;
    bool quit = false;
    bool lockCam = false;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 newTarget;
    glm::vec3 movement = glm::vec3(0.0f);;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float movementSpeed = 2.5f;

    int relX;
    int relY;

    int mouseX;
    int mouseY;

    int accumX = 0;
    int accumY = 0;

    bool mouseGrabbed = true;

    voxFile testVoxModel = openVoxFile("./voxelFiles/debugFile.vv");
    Voxel* voxels = makeVoxels(testVoxModel);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);

    while(!quit){
        newTarget = getTarget(position, target, up, accumX, accumY, lockCam);
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button){
                        case SDL_BUTTON_LEFT:
                            mouseGrabbed = true;
                            lockCam = false;
                            SDL_SetWindowGrab(window, SDL_TRUE);
                            SDL_ShowCursor(SDL_DISABLE);
                            break;
                    }
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym){
                        case SDLK_ESCAPE:
                            mouseGrabbed = false;
                            lockCam = true;
                            SDL_SetWindowGrab(window, SDL_FALSE);
                            SDL_ShowCursor(SDL_ENABLE);
                            break;

                        case SDLK_w:
                            movement += glm::normalize(newTarget - position) * movementSpeed;
                            break;

                        case SDLK_s:
                            movement -= glm::normalize(newTarget - position) * movementSpeed;
                            break;

                        case SDLK_a:
                            movement -= glm::normalize(glm::cross(newTarget - position, up)) * movementSpeed;
                            break;

                        case SDLK_d:
                            movement += glm::normalize(glm::cross(newTarget - position, up)) * movementSpeed;
                            break;
                    }
                    break;

                case SDL_MOUSEMOTION:
                    relX = event.motion.xrel;
                    relY = event.motion.yrel;

                    accumX += relX;
                    accumY += relY;

                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    int newMouseX = mouseX + relX;
                    int newMouseY = mouseY + relY;

                    if(mouseGrabbed && (newMouseX < 0 || newMouseX > WINDOW_WIDTH || newMouseY < 0 || newMouseY > WINDOW_HEIGHT)){
                        SDL_WarpMouseInWindow(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                    }
                    break;

                }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        position += movement;
        movement = glm::vec3(0.0f);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glm::mat4 viewport = getViewPortMatrix((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, position, newTarget, up);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glMultMatrixf(glm::value_ptr(viewport));
        //printf("Mouse X: %d Mouse Y: %d\n", newMouseX, newMouseY);

        printf("Position: %s, Target: %s, Front: %s\n", glm::to_string(position).c_str(), glm::to_string(newTarget).c_str(), glm::to_string(glm::normalize(target - position)).c_str());

        renderVoxels(voxels, testVoxModel);

        SDL_GL_SwapWindow(window);
    }

    glDisable(GL_DEPTH_TEST);
}

