all:
	gcc	-c	src/main.h
	gcc	-c	src/init.c
	gcc	-c	src/deinit.c
	gcc	-c	src/main.c
	gcc	-c	src/draw.c
	gcc	-c	src/input.c
	gcc	-o	ENLIL_ENRAGED	main.o	init.o	deinit.o	draw.o	input.o	-lmingw32	-lSDL2Main	-lSDL2	-lSDL2_image	-lSDL2_ttf	-lSDL2_mixer	-ljansson
	rm	"main.o"
	rm	"init.o"
	rm	"draw.o"
	rm	"deinit.o"
	rm	"input.o"
	rm	"src/main.h.gch"

debug:
	gcc	-c	-g	src/main.h
	gcc	-c	-g	src/init.c
	gcc	-c	-g	src/deinit.c
	gcc	-c	-g	src/main.c
	gcc	-c	-g	src/draw.c
	gcc	-c	-g	src/input.c
	gcc	-o	ENLIL_ENRAGED_DEBUG	main.o	init.o	deinit.o	draw.o	input.o	-lmingw32	-lSDL2Main	-lSDL2	-lSDL2_image	-lSDL2_ttf	-lSDL2_mixer	-ljansson	-g
	rm	"main.o"
	rm	"init.o"
	rm	"draw.o"
	rm	"deinit.o"
	rm	"input.o"
	rm	"src/main.h.gch"