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


char* loadShader(const char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open shader file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (buffer == NULL) {
        printf("Failed to allocate memory for shader file: %s\n", filename);
        fclose(file);
        return NULL;
    }

    size_t readSize = fread(buffer, 1, size, file);
    if (readSize != size) {
        printf("Failed to read shader file: %s\n", filename);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char* argv[]){
    // SDL and openGL context initialization
    SDL_Window* window = NULL;
    SDL_GLContext glContext = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    }

    window = SDL_CreateWindow("Voxel Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN| SDL_WINDOW_RESIZABLE);
    glContext = SDL_GL_CreateContext(window);

    GLenum glewError = glewInit();

    if(glewError != GLEW_OK){
        printf("Error initializing GLEW! %s", glewGetErrorString(glewError));
    }

    SDL_Event event;

    bool quit = false;

    // Depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Shader programs

   float cubeVertices[] = {
    // Front
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // Back
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

    // Left
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    // Right
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,

    // Top
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Bottom
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f
};


    GLuint cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    char* vertexShaderFile = loadShader("./shaders/vertexShader.glsl");
    char* fragmentShaderFile = loadShader("./shaders/fragmentShader.glsl");

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderFile, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderFile, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint viewMatrixPos = glGetUniformLocation(shaderProgram, "viewMatrix");
    GLint projectionMatrixPos = glGetUniformLocation(shaderProgram, "projectionMatrix");
    GLint modelMatrixPos = glGetUniformLocation(shaderProgram, "modelMatrix");

    // Camera
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Voxel file loading
    voxFile debugVoxelFile = openVoxFile("./voxelFiles/debugFile.vv");
    Voxel* debugVoxels = makeVoxels(debugVoxelFile);

    while(!quit){
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }


        glm::mat4 modelMatrix = getViewMatrix(position, target, up);
        glm::mat4 projectionMatrix = getProjectionMatrix(WINDOW_WIDTH, WINDOW_HEIGHT);
        glm::mat4 viewPortMatrix = getViewPortMatrix(WINDOW_WIDTH, WINDOW_HEIGHT, position, target, up);

        glUniformMatrix4fv(modelMatrixPos, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixPos, 1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixPos, 1, GL_FALSE, &viewPortMatrix[0][0]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        renderVoxels(debugVoxels, debugVoxelFile, shaderProgram, cubeVBO);

        glUseProgram(shaderProgram);

        SDL_GL_SwapWindow(window);

    }

}


