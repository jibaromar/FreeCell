sdl2_config = -I/usr/include/SDL2 -D_REENTRANT -lSDL2

prog: main.c
	gcc main.c $(sdl2_config) -o prog
clean:
	rm prog