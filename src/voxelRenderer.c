#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "voxelRenderer.h"

#define false 0
#define true 1


voxFile openVoxFile(const char* filename){
    FILE* preRead = fopen(filename, "r");
    char preReadBuffer[300];

    static voxFile out;

    // Read the file to get the voxel count and palette count

    while(fgets(preReadBuffer, sizeof(preReadBuffer), preRead)){
        printf("%s", preReadBuffer);

        if(strstr(preReadBuffer, "VOXELS:")){
            sscanf(preReadBuffer, "VOXELS: %d", &out.voxelCount);
        }

        if(strstr(preReadBuffer, "PALETTE:")){
            sscanf(preReadBuffer, "PALETTE: %d", &out.paletteCount);
        }
    }

    fclose(preRead);

    printf("Voxel count: %d\n", out.voxelCount);
    printf("Palette count: %d\n", out.paletteCount);

    // Initialize the arrays

    out.x = (int*)malloc(sizeof(int) * out.voxelCount); //
    out.y = (int*)malloc(sizeof(int) * out.voxelCount); // Initialize the position arrays
    out.z = (int*)malloc(sizeof(int) * out.voxelCount); //

    out.r = (int*)malloc(sizeof(int) * out.paletteCount); //
    out.g = (int*)malloc(sizeof(int) * out.paletteCount); // Initialize the color arrays
    out.b = (int*)malloc(sizeof(int) * out.paletteCount); //

    out.colorIndex = (int*)malloc(sizeof(int) * out.voxelCount); // Initialize the color index array

    // Read the file again to get the data
    FILE* mainRead = fopen(filename, "r");
    char mainReadBuffer[300];

    bool voxelSection = false;
    bool paletteSection = false;

    int x, y, z, r, g, b, colorIndex, voxelIndex, paletteIndex;

    while(fgets(mainReadBuffer, sizeof(mainReadBuffer), mainRead)){
        if(strstr(mainReadBuffer, "VOXELS:")){
            voxelSection = true;
            paletteSection = false;
            continue;
        }

        if(strstr(mainReadBuffer, "PALETTE:")){
            voxelSection = false;
            paletteSection = true;
            continue;
        }

        if(strstr(mainReadBuffer, "#END")){
            voxelSection = false;
            paletteSection = false;
            break;
        }

        if(voxelSection){
            sscanf(mainReadBuffer, "%d, %d, %d, %d, %d;", &voxelIndex, &x, &y, &z, &colorIndex);
            out.x[voxelIndex] = x;
            out.y[voxelIndex] = y;
            out.z[voxelIndex] = z;
            out.colorIndex[voxelIndex] = colorIndex;
        }

        if(paletteSection){
            sscanf(mainReadBuffer, "%d, %d, %d, %d;", &paletteIndex, &r, &g, &b);
            out.r[paletteIndex] = r;
            out.g[paletteIndex] = g;
            out.b[paletteIndex] = b;
        }
    }

    fclose(mainRead);

    return out;
}

Voxel* makeVoxels(voxFile file){
    Voxel* out = (Voxel*)malloc(sizeof(Voxel) * file.voxelCount);
    for(int i = 0; i < file.voxelCount; i++){
        out[i].x = file.x[i];
        out[i].y = file.y[i];
        out[i].z = file.z[i];
        out[i].r = file.r[file.colorIndex[i]];
        out[i].g = file.g[file.colorIndex[i]];
        out[i].b = file.b[file.colorIndex[i]];
    }

    return out;
}

void renderVoxels(Voxel* voxels, voxFile file, unsigned int shaderProgram, GLuint VBO){

    int vertexPosLoc = glGetAttribLocation(shaderProgram, "aPos");

    for(int i = 0; i < file.voxelCount; i++){
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(voxels[i].x, voxels[i].y, voxels[i].z));

        int modelMatrixLoc = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        int colorLoc = glGetUniformLocation(shaderProgram, "color");
        glUniform3f(colorLoc, voxels[i].r, voxels[i].g, voxels[i].b);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(vertexPosLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(vertexPosLoc);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisableVertexAttribArray(vertexPosLoc);

        glBindBuffer(GL_ARRAY_BUFFER, 0);


    }
}
