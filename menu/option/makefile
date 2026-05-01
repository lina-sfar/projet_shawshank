prog: main.o menu.o
	gcc main.o menu.o -o prog `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -g

main.o: main.c
	gcc -c main.c `sdl2-config --cflags` -g

menu.o: menu.c
	gcc -c menu.c `sdl2-config --cflags` -g

clean:
	rm -f *.o prog
