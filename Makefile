CXX = g++
CFLAGS = -Wall -Wextra -std=c17 `pkg-config --cflags sdl2 SDL2_ttf glew` -I./include
CPPFLAGS = -Wall -Wextra -std=c++17 `pkg-config --cflags sdl2 SDL2_ttf glew glm`
LIBS = `pkg-config --libs sdl2 SDL2_ttf glew` -framework OpenGL

CSOURCES = ./src/main.c ./src/voxelRenderer.c
CPPSOURCES = ./src/Camera.cpp

VOXFILE = debugFile.vv
SHADERS = ./src/shaders/vertexShader.glsl ./src/shaders/fragmentShader.glsl

.PHONY: all build clean

all: build

build:
	mkdir -p build
	$(CXX) $(CSOURCES) $(CPPSOURCES) -o build/main $(CFLAGS) $(CPPFLAGS) $(LIBS)
	
	mkdir -p build/voxelFiles
	mkdir -p build/shaders
	
	cp -p $(VOXFILE) build/voxelFiles/
	cp -p $(SHADERS) build/shaders/



	
clean:
	rm -rf build/

