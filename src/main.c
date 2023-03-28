#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float movementSpeed = 2.5f;


    voxFile testVoxModel = openVoxFile("test.vv");
    Voxel* voxels = makeVoxels(testVoxModel);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while(!quit){
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym){
                        case SDLK_w:
                            position += movementSpeed * glm::normalize(target - position);
                            break;

                        case SDLK_s:
                            position -= movementSpeed * glm::normalize(target - position);
                            break;

                        case SDLK_a:
                            position -= glm::normalize(glm::cross(target - position, up)) * movementSpeed;
                            break;

                        case SDLK_d:
                            position += glm::normalize(glm::cross(target - position, up)) * movementSpeed;
                            break;
                    }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glm::mat4 viewport = getViewPortMatrix((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, position, target, up);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glMultMatrixf(glm::value_ptr(viewport));

        printf("%s\n", glm::to_string(viewport).c_str());

        renderVoxels(voxels, testVoxModel);

        SDL_GL_SwapWindow(window);
    }

    glDisable(GL_DEPTH_TEST);
}

