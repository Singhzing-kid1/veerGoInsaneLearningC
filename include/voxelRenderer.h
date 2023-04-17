#ifndef VOXELRENDERER_H
#define VOXELRENDERER_H

typedef struct{
    int *x, *y, *z;
    int *r, *g, *b, *colorIndex;
    int voxelCount, paletteCount;
} voxFile;

typedef struct{
    int x, y, z;
    int r, g, b;
} Voxel;


voxFile openVoxFile(const char* filename);
Voxel* makeVoxels(voxFile file);
void renderVoxels(Voxel* voxels, voxFile file, unsigned int shaderProgram, GLuint VBO);


#endif
