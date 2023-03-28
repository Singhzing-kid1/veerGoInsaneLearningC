CXX = g++
CFLAGS = -Wall -Wextra -std=c17 `pkg-config --cflags sdl2 SDL2_ttf glew` -I./include
CPPFLAGS = -Wall -Wextra -std=c++17 `pkg-config --cflags sdl2 SDL2_ttf glew glm`
LIBS = `pkg-config --libs sdl2 SDL2_ttf glew` -framework OpenGL

CSOURCES = ./src/main.c ./src/voxelRenderer.c
CPPSOURCES = ./src/Camera.cpp

VOXFILE = debugFile.vv

.PHONY: all build clean

all: build

build:
	$(CXX) -v  $(CSOURCES) $(CPPSOURCES) -o build/main $(CFLAGS) $(CPPFLAGS) $(LIBS)
	
	cp -p $(VOXFILE) build/

clean:
	rm -f build/main build/*.o build/*.vv

