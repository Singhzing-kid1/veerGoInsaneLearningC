
CC = gcc
CFLAGS = -Wall -Wextra -std=c17 `pkg-config --cflags sdl2 glew cglm` 
LIBS = `pkg-config --libs sdl2 glew cglm` -framework OpenGL -v

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(OBJ) $(EXEC)

