CC = gcc
CFLAGS = -Wall -Wextra -std=c17 `pkg-config --cflags sdl2 SDL2_ttf`
LIBS = `pkg-config --libs sdl2 SDL2_ttf`

SRC = *.c
OBJ = $(SRC:.c=.o)
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(OBJ) $(EXEC)
