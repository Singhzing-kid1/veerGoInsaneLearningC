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

GLfloat cubeVertices[] = {
    // Front face
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    // Back face
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    // Right face
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
    // Left face
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    // Bottom face
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    // Top face
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
};
GLuint cubeIndicies[] = {
    0, 1, 2, 2, 3, 0, // Front face
    4, 5, 6, 6, 7, 4, // Back face
    8, 9, 10, 10, 11, 8, // Right face
    12, 13, 14, 14, 15, 12, // Left face
    16, 17, 18, 18, 19, 16, // Bottom face
    20, 21, 22, 22, 23, 20, // Top face
};

 int main(int argc, char* argv[]){
    // Initialize SDL
    SDL_Window* window;
    SDL_GLContext glContext;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Voxel Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);

    if (glContext == NULL) {
        printf("Failed to create OpenGL context: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    if (glewError != GLEW_OK) {
        printf("Failed to initialize GLEW: %s\n", glewGetErrorString(glewError));
        return 1;
    }

    // Initialize OpenGL

    // Enable depth testing

    // SDL event
    SDL_Event event;

    // Shaders

    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* vertexShaderSource = loadShader("./shaders/vertexShader.glsl");
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    free(vertexShaderSource);

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* fragmentShaderSource = loadShader("./shaders/fragmentShader.glsl");
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    free(fragmentShaderSource);

    // Shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLuint VBO, IBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndicies), cubeIndicies, GL_STATIC_DRAW);

    GLint mvpLocation = glGetAttribLocation(shaderProgram, "uModelViewProjectionMatrix");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Camera

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Main loop
    bool quit = false;

    while(!quit){
        // Handle Events
        while(SDL_PollEvent(&event) != 0){
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera
        glm::mat4 mvpMatrix = getViewPortMatrix((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, position, target, up);

        // Set MVP matrix
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // Draw cube
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);

        // Update screen

        SDL_GL_SwapWindow(window);

    }

}


