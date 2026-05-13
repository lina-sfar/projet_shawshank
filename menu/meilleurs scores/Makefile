CC = gcc
CFLAGS = -Wall -g
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

all:
	gcc -o shawshank main.c menu.c scores.c $(LIBS)

clean:
	rm -f shawshank scores.dat

run:
	./shawshank
