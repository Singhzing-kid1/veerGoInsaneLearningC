CXX = g++
CFLAGS = -Wall -Wextra -std=c17 `pkg-config --cflags sdl2 SDL2_ttf glew` -I./include
CPPFLAGS = -Wall -Wextra -std=c++17 `pkg-config --cflags sdl2 SDL2_ttf glew glm`
LIBS = `pkg-config --libs sdl2 SDL2_ttf glew` -framework OpenGL

CSOURCES = main.c voxelRenderer.c
CPPSOURCES = Camera.cpp

all: build

build:
	$(CXX) -v  $(CSOURCES) $(CPPSOURCES) -o main $(CFLAGS) $(CPPFLAGS) $(LIBS)


clean:
	rm -f main *.o
