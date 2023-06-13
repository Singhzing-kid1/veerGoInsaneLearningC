#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL_opengl.h>

#include <cglm/cglm.h>

#define false 0
#define true 1

int main(int argc, char* argv[]){
    SDL_Window* window;
    SDL_GLContext context;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if(window == NULL){
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(0);

    if(context == NULL){
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    if(glewError != GLEW_OK){
        printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        return 1;
    }


    SDL_Event e;

    int quit = false;

    float vertecies[] = {
        0.5f, 0.5f, 0.5f, 0.8f, 0.0f, 0.7f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.3f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.7f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.7f, 0.7f
    };

    unsigned int indicies[] = {
        2, 0, 1,
        2, 3, 1,
        3, 1, 5,
        3, 7, 5,
        7, 5, 4,
        7, 6, 4,
        6, 4, 0,
        6, 2, 0,
        1, 5, 4,
        1, 0, 4,
        2, 3, 7,
        2, 6, 7
    };

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aColor;\n"
                                     "out vec3 ourColor;\n"
                                     "uniform mat4 model;\n"
                                     "uniform mat4 view;\n"
                                     "uniform mat4 projection;\n"
                                     "void main() {\n"
                                     "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                     "    ourColor = aColor;\n"
                                     "}\0";

    const char *fragmentShaderSource =  "#version 330 core\n"
                                        "out vec4 FragColor;\n"
                                        "in vec3 ourColor;\n"
                                        "void main() {\n"
                                        "    FragColor = vec4(ourColor, 1.0f);\n"
                                        "}\0";

    printf("%s\n", glGetString(GL_VERSION));

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    glEnable(GL_DEPTH_TEST);

    vec3 cameraPos = {0.0f, 0.0f, 3.0f};
    vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
    vec3 cameraFront = {0.0f, 0.0f, -1.0f};


    while(!quit){

        vec3 cameraDirection;

        glm_vec3_sub(cameraTarget, cameraPos, cameraDirection);
        glm_normalize(cameraDirection);

        vec3 up = {0.0f, 1.0f, 0.0f};
        vec3 cameraRight;

        glm_cross(cameraDirection, up, cameraRight);
        glm_normalize(cameraRight);

        vec3 cameraUp;

        glm_cross(cameraRight, cameraDirection, cameraUp);

        vec3 cameraFrontPlusPos;
        glm_vec3_add(cameraFront, cameraPos, cameraFrontPlusPos);

        float cameraSpeed = 0.05f;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        {
                            vec3 scaledFront;
                            glm_vec3_scale(cameraFront, cameraSpeed, scaledFront);
                            glm_vec3_add(cameraPos, scaledFront, cameraPos);
                        }
                        break;
                    case SDLK_s:
                        {
                            vec3 scaledFront;
                            glm_vec3_scale(cameraFront, cameraSpeed, scaledFront);
                            glm_vec3_sub(cameraPos, scaledFront, cameraPos);
                        }
                        break;
                    case SDLK_a:
                        {
                            vec3 right;
                            glm_vec3_cross(cameraFront, cameraUp, right);
                            glm_normalize(right);
                            glm_vec3_scale(right, cameraSpeed, right);
                            glm_vec3_sub(cameraPos, right, cameraPos);
                        }
                        break;
                    case SDLK_d:
                        {
                            vec3 right;
                            glm_vec3_cross(cameraFront, cameraUp, right);
                            glm_normalize(right);
                            glm_vec3_scale(right, cameraSpeed, right);
                            glm_vec3_add(cameraPos, right, cameraPos);
                        }
                        break;
                }
            }
        }



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 model;
        float angle = SDL_GetTicks() / 1000.0f * 45.0f;
        vec3 axis = {0.5f, 1.0f, 0.0f};

        mat4 view;

        mat4 projection;
        float fov = 45.0f;
        float aspect = 640.0f / 480.0f;
        float near = 0.1f;
        float far = 100.0f;

        glm_mat4_identity(model);
        glm_rotate(model, glm_rad(angle), axis);

        glm_mat4_identity(view);
        glm_lookat(cameraPos, cameraFrontPlusPos, cameraUp, view);

        glm_perspective(glm_rad(fov), aspect, near, far, projection);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

}
